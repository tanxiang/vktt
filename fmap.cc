#include "fmap.hh"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

namespace tt{

std::pair<FileMap,size_t> fileMapBuf(const char* fileName)
{
	auto fd = open(fileName, O_RDONLY);
	struct stat fileStat;
	fstat(fd, &fileStat);
	auto bufPair = std::pair<FileMap,size_t> {
		FileMap{
			mmap(NULL, fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0),
			[=](void* p){
				munmap(p,fileStat.st_size);
				std::cout<<"munmap"<<fileStat.st_size<<'\n';
			}
		},
		fileStat.st_size
	};
	std::cout<<"mmap"<<fileStat.st_size<<'\n';
	close(fd);
	return bufPair;
}

}//namespace
