#include "fmap.hh"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

namespace tt{

std::unique_ptr<void,std::function<void(void*)>> fileMapBuf(const char* fileName,size_t* fileSize)
{
	auto fd = open(fileName, O_RDONLY);
	struct stat fileStat;
	fstat(fd, &fileStat);
	if(fileSize) *fileSize = fileStat.st_size;
	auto buf = std::unique_ptr<void,std::function<void(void*)>>{
		mmap(NULL, fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0),
		[=](void* p){
			munmap(p,fileStat.st_size);
			std::cout<<"munmap"<<fileStat.st_size<<'\n';
		}
	};
	close(fd);
	return buf;
}

}//namespace
