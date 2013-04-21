#pragma once
#include <SFML/System/InputStream.hpp>
struct PHYSFS_File;//forward
namespace edy{
	namespace io{
		class PhysInputStream : public sf::InputStream
		{
		public:
										PhysInputStream(const char * filename=0x0);
			virtual						~PhysInputStream();
			virtual	sf::Int64			read(void * data, sf::Int64 size);
			virtual	sf::Int64			seek(sf::Int64 position);
			virtual	sf::Int64			tell();
			virtual	sf::Int64			getSize();
			bool						open(const char * filename);
			bool						isOpen()const;
			void						close();
			bool						setBufferSize(sf::Int64 buffersize);
		private:
			PHYSFS_File *				m_File;
		};
	}
}