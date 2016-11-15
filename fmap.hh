#pragma once
#include <iostream>
#include <experimental/filesystem>
#include <memory>

namespace tt{
using FileMap = std::unique_ptr<void,std::function<void(void*)>>;
FileMap fileMapBuf(const char* fileName,size_t* fileSize = nullptr);

}//namespace
