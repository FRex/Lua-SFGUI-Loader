#include "LuaTable.h"
#include <sstream>
#include "lua/lua.hpp"
#include <cassert>
#include <cctype>
namespace {
	std::string indent(int amount)
	{
		return std::string(amount,'\t');
	}
	//slightly modified luas addquoted from lstrlib.c that gets used with %q
	//see lua license for details
	std::string qEscape(const char * str)
	{
		std::string ret="\"";
		while (*str)
		{
			if (*str=='"'||*str=='\\'||*str=='\n')
			{
				ret+='\\';
				ret+=*str;
			}
			else if (iscntrl(static_cast<unsigned char>(*str)))
			{
				char buff[10];
				if (!isdigit(static_cast<unsigned char>(*(str+1))))
				{
					sprintf(buff,"\\%d",static_cast<int>(static_cast<unsigned char>(*str)));
				}
				else
				{
					sprintf(buff,"\\%03d",static_cast<int>(static_cast<unsigned char>(*str)));
				}
				ret+=buff;
			}
			else
			{
				ret+=*str;
			}
			str++;
		}
		ret+="\"";
		return ret;
	}
	//end of Lua modified lib code
}
namespace edy{
	namespace script{
		LuaTable::LuaTable():m_Type(None)
		{

		}
		bool LuaTable::loadFromString(const char * string)
		{
			bool ret;
			lua_State * L=luaL_newstate();
			if(luaL_dostring(L,string))
			{
				ret=false;
			}
			else
			{
				lua_pushglobaltable(L);
				loadInto(L,-1,*this);
				ret=true;
			}
			lua_close(L);
			return ret;
		}
		bool LuaTable::loadFromState(lua_State * L,int index)
		{
			if(!lua_istable(L,index)) return false;
			loadInto(L,index,*this);
			return true;
		}
		std::string LuaTable::formatted()const
		{
			std::ostringstream str;
			format(str,0);
			return str.str();
		}
		void LuaTable::format(std::ostream& out,int in)const
		{
			switch(m_Type)
			{
			case None:		out<<"nil"; return;//shouldnt ever happen??
			case Boolean:	out<<(m_Boolean?"true":"false"); return;//std::boolalpha is not required
			case Number:	out<<m_Number; return;
			case String:	out<<qEscape(m_String.c_str()); return;
			}
			if(m_Map.size()==0)
			{
				out<<"nil";//must do that or wed have dangling =
				return;
			}
			if(in!=0) out<<"{\n";//dont surround _G
			bool table=false;
			for(int i=1;i<=m_Map.size();++i)
			{
				if(m_Map.find(core::varToString(i))==m_Map.end())
				{
					table=true;//found non 1..n int key! this is not plain array
					break;
				}
			}
			for(auto it=m_Map.begin();it!=m_Map.end();)
			{
				if(in!=0) out<<indent(in+1);
				if(table) out<<it->first<<" = ";//for tables print key=
				it->second.format(out,in+1);//print value..
				if(++it!=m_Map.end()&&in!=0) out<<",";//print separting ,s except in _G
				out<<std::endl;
			}
			if(in!=0) out<<indent(in)<<"}";//dont surround _G
		}
		//read and insert/write:
		LuaTable& LuaTable::operator[](int index)
		{
			m_Type=Array;
			return (*this)[core::varToString(index)];
		}
		LuaTable& LuaTable::operator[](const std::string& key)
		{
			m_Type=Array;
			return m_Map[key];
		}
		//read:
		const LuaTable& LuaTable::operator()(int index)const
		{
			if(index<0) index=getSize()-index+1;//pseudo indices that are negative, like in lua
			return (*this)(core::varToString(index));
		}
		const LuaTable& LuaTable::operator()(const std::string& key)const
		{
			auto it=m_Map.find(key);
			if(m_Type!=Array||it==m_Map.end())
			{
				static const LuaTable empty;
				return empty;
			}
			return it->second;
		}
		//check for value and evenutally get it:
		bool LuaTable::tryBool(bool& out)const
		{
			if(m_Type==Boolean)
			{
				out=m_Boolean;
				return true;
			}
			return false;
		}
		bool LuaTable::tryString(std::string& out)const
		{
			if(m_Type==String)
			{
				out=m_String;
				return true;
			}
			return false;
		}
		//get value or default if type doesnt match
		double LuaTable::asNumber()const
		{
			if(m_Type==Number) return m_Number;
			return 0.0;
		}
		bool LuaTable::asBoolean()const
		{
			if(m_Type==Boolean) return m_Boolean;
			return false;
		}
		const std::string& LuaTable::asString()const
		{
			static const std::string empty;
			if(m_Type==String) return m_String;
			return empty;
		}
		//assignments
		LuaTable& LuaTable::operator=(bool boolean)
		{
			m_Map.clear();
			m_Type=Boolean;
			m_Boolean=boolean;
			return *this;
		}
		LuaTable& LuaTable::operator=(double number)
		{
			m_Map.clear();
			m_Type=Number;
			m_Number=number;
			return *this;
		}
		LuaTable& LuaTable::operator=(const LuaTable& value)
		{
			m_Map=value.m_Map;
			m_String=value.m_String;
			m_Boolean=value.m_Boolean;
			m_Number=value.m_Number;
			m_Type=value.m_Type;
			return *this;
		}
		LuaTable& LuaTable::operator=(const std::string& str)
		{
			m_Map.clear();
			m_Type=String;
			m_String=str;
			return *this;
		}
		LuaTable& LuaTable::operator=(const char * str)
		{
			m_Map.clear();
			m_Type=String;
			m_String=str;
			return *this;
		}
		void LuaTable::remove(int index)
		{
			if(index<0) index=getSize()-index+1;
			remove(core::varToString(index));
		}
		void LuaTable::remove(const std::string& key)
		{
			auto it=m_Map.find(key);
			if(it!=m_Map.end())
			{
				m_Map.erase(it);
			}
		}
		void LuaTable::loadInto(lua_State * L,int index,LuaTable& table)
		{
			table.m_Type=Array;
			lua_pushvalue(L,index);
			lua_pushnil(L);
			std::string key;
			while(lua_next(L,-2)!=0)
			{
				//key:
				if(lua_isnumber(L,-2))
				{
					key=core::varToString(lua_tointeger(L,-2));
					auto eh=lua_gettop(L);
				}
				else if(lua_isstring(L,-2))
				{
					key=lua_tolstring(L,-2,0x0);
				}
				else{assert(false);}//we shouldnt get here, ever..
				//value:
				if(lua_isboolean(L,-1))
				{

					table[key]=(lua_toboolean(L,-1)!=0);
				}
				else if(lua_isnumber(L,-1))
				{

					table[key]=lua_tonumber(L,-1);
				}
				else if(lua_isstring(L,-1))
				{
					table[key]=lua_tolstring(L,-1,0x0);
				}
				else if(lua_istable(L,-1))
				{
					loadInto(L,-1,table[key]);
				}
				lua_pop(L,1);
			}
			lua_pop(L,1);
		}//::loadInto
		bool LuaTable::operator==(const std::string& value)const
		{
			if(m_Type==String) return m_String==value;
			return false;
		}
		bool LuaTable::operator==(const char * value)const
		{
			return (*this)==std::string(value);
		}
		bool LuaTable::operator==(double value)const
		{
			if(m_Type==Number) return m_Number==value;
			return false;
		}
		bool LuaTable::operator==(bool value)const
		{
			if(m_Type==Boolean) return m_Boolean==value;
			return false;
		}
		double LuaTable::forceNumber()const
		{
			switch(m_Type)
			{
			default:				return 0.0;
			case Boolean:			return m_Boolean?1.0:0.0;
			case Number:			return m_Number;
			case String:			return core::stringToVar<double>(m_String,0.0);
			}
		}
		bool LuaTable::forceBoolean()const
		{
			switch(m_Type)
			{
			default:				return false;
			case Boolean:			return m_Boolean;
			case Number:			return m_Number>=0.5;//like in gml.. quite stupid and arbitrary condition..
			case String:			return core::stringToVar<bool>(m_String,false);
			}
		}
		std::string LuaTable::forceString()const
		{
			switch(m_Type)
			{
			default:				return "";
			case Boolean:			return core::varToString(m_Boolean);
			case Number:			return core::varToString(m_Number);
			case String:			return m_String;
			}
		}
	}
}
