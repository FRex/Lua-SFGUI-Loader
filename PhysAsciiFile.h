#pragma once
#include <vector>
/*
ENTIRE file mapped to ascii, this just crap over the memory,
todo: create streaming classes for lua and normal text,
*/
namespace edy{
	namespace io{
		class PhysAsciiFile
		{
		public:
									PhysAsciiFile(const char * filename=0x0){if(filename) load(filename);}
			const char *			getContents()const{return loaded()?m_Buffer.data():0x0;}
			void					purge(){std::vector<char>().swap(m_Buffer);}
			bool					load(const char * filename);
			bool					loaded()const{return !m_Buffer.empty();}
			static bool				save(const char * filename,const char * contents,int length=0,bool append=false);
		private:
			std::vector<char>		m_Buffer;
		};
	}
}