#pragma once
#include <sstream>
namespace edy{
	namespace core{
		template <typename T> std::string varToString(T targ)
		{
			std::ostringstream ss;
			ss<<std::boolalpha<<targ;
			return ss.str();
		}
		template <typename T> T stringToVar(const std::string& targ,T def=T())
		{
			std::istringstream ss(targ);
			T ret;
			if(ss>>std::boolalpha>>ret) return ret;
			return def;//not sure if >> doesnt modify in case of failure..
		}
	}
}