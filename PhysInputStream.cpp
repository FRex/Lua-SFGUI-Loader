#include "PhysInputStream.h"
#include "physfs/physfs.h"
namespace edy{
	namespace io{
		PhysInputStream::PhysInputStream(const char * filename):m_File(0x0)

		{
			if(filename) open(filename);
		}
		PhysInputStream::~PhysInputStream()
		{
			close();
		}
		sf::Int64 PhysInputStream::read(void * data,sf::Int64 size)
		{
			if(!m_File) return -1;
			return PHYSFS_read(m_File,data,1,static_cast<PHYSFS_uint32>(size));
		}
		sf::Int64 PhysInputStream::seek(sf::Int64 position)
		{
			if(!m_File) return -1;
			if(PHYSFS_seek(m_File,position))
			{
				return PHYSFS_tell(m_File);
			}
			else
			{
				return -1;
			}
		}
		sf::Int64 PhysInputStream::tell()
		{
			if(!m_File) return -1;
			return PHYSFS_tell(m_File);
		}
		sf::Int64 PhysInputStream::getSize()
		{
			if(!m_File) return -1;
			return PHYSFS_fileLength(m_File);
		}
		bool PhysInputStream::open(const char * filename)
		{
			close();
			m_File=PHYSFS_openRead(filename);
			return isOpen();
		}
		bool PhysInputStream::isOpen()const
		{
			return m_File!=0x0;
		}
		void PhysInputStream::close()
		{
			if(m_File)
			{
				PHYSFS_close(m_File);
				m_File=0x0;
			}
		}
		bool PhysInputStream::setBufferSize(sf::Int64 buffersize)
		{
			if(!m_File||!PHYSFS_setBuffer(m_File,buffersize)) return false;
			return true;
		}
	}
}