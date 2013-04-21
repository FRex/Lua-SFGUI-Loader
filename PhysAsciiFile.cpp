#include "PhysAsciiFile.h"
#include "physfs/physfs.h"
#include <cstring>
namespace edy{
	namespace io{
		bool PhysAsciiFile::load(const char * filename)
		{
			purge();
			PHYSFS_File * file=PHYSFS_openRead(filename);
			if(!file) return false;
			m_Buffer.resize(PHYSFS_fileLength(file)+1);
			PHYSFS_read(file,m_Buffer.data(),1,m_Buffer.size()-1);
			PHYSFS_close(file);
			m_Buffer.back()='\0';
			return true;
		}
		bool PhysAsciiFile::save(const char * filename,const char * contents,int length,bool append)
		{
			if(length<=0) length=std::strlen(contents);
			if(length==0) return false;
			PHYSFS_File * file=append?PHYSFS_openAppend(filename):PHYSFS_openWrite(filename);
			if(!file) return false;
			PHYSFS_write(file,contents,1,length);
			PHYSFS_close(file);
			return true;
		}
	}
}