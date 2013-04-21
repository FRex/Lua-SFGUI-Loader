#pragma once
#include <map>
#include <string>
//#include <edy/core/varToString.h>
#include "varToString.h"
/*

array was removed, now at write time itll check if keys are only ints
table means that keys are strings but at least one of them isnt pure int
the rest mean that map is empty and one of member values holds the actuall value
operators [] allow reading and writing like table[1]="yay";
operators () allow just reading, and return empty value if they dont have 
specified element, const overloading didnt do it here beacuse these
are two different ops
array indexing is, as in lua 1..n or -1..-n, not 0..n-1
somewhat inspired by LuaTable lib but it doesn't copy a single line from it,
just general idea is same
*/
struct lua_State;
namespace edy{
	namespace script{
		class LuaTable
		{
		public:
			typedef std::map<std::string,LuaTable>	Map;
			typedef Map::iterator					Iterator;
			typedef Map::const_iterator				ConstIterator;
			enum Type								{Array,String,Number,Boolean,None};
													LuaTable();
			bool									loadFromString(const char * string);
			bool									loadFromState(lua_State * L, int index);
			std::string								formatted()const;
			//read and insert/write:
			LuaTable&								operator[](int index);
			LuaTable&								operator[](const std::string& key);
			//read:
			const LuaTable&							operator()(int index)const;
			const LuaTable&							operator()(const std::string& key)const;
			//check for value and evenutally get it:
			bool									tryBool(bool& out)const;
			bool									tryString(std::string& out)const;
			template <typename T> bool				tryNumber(T& out)const;
			//get value or default if type doesnt match
			double									asNumber()const;//default 0.0
			bool									asBoolean()const;//default false
			const std::string&						asString()const;//default ""
			//assignments
			LuaTable&								operator=(bool boolean);
			LuaTable&								operator=(double number);
			LuaTable&								operator=(const LuaTable& value);
			LuaTable&								operator=(const std::string& str);
			LuaTable&								operator=(const char * str);

			//get table content type:
			Type									getType()const{return m_Type;}
			//get size of table:
			unsigned								getSize()const{return m_Map.size();}
			//clear the value completely:
			void									clear(){m_Type=None;m_Map.clear();}
			//remove value by key:
			void									remove(int index);
			void									remove(const std::string& key);
			//get iterators:
			Iterator								begin(){return m_Map.begin();}
			Iterator								end(){return m_Map.end();}
			ConstIterator							begin()const{return m_Map.begin();}
			ConstIterator							end()const{return m_Map.end();}
			//comparsion opearators
			bool									operator==(const std::string& value)const;
			bool									operator==(const char * value)const;//we are victims of ptr->bool coercion nonsense..
			bool									operator==(double value)const;
			bool									operator==(bool value)const;
			//coercers, will do lolzy conversions, careful..:
			double									forceNumber()const;
			bool									forceBoolean()const;
			std::string								forceString()const;							
		private:
			void									format(std::ostream& out,int indent)const;
			static void								loadInto(lua_State * L,int index,LuaTable& table);
			bool									m_Boolean;
			std::string								m_String;
			double									m_Number;
			Type									m_Type;
			Map										m_Map;
		};
		template <typename T> bool LuaTable::tryNumber(T& out)const
		{
			if(m_Type==Number)
			{
				out=static_cast<T>(m_Number);
				return true;
			}
			return false;
		}
	}
}