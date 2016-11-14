#pragma once
#include <iostream>
#include <experimental/filesystem>
#include <memory>
#include <sys/mman.h>

namespace tt{
class fileMapBuf{
	std::unique_ptr<char[]> buf;
};
}//namespace
