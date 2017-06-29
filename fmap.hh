#pragma once
#include <iostream>
#include <experimental/filesystem>
#include <memory>
#include <utility>
#include <functional>
namespace tt{
using FileMap = std::unique_ptr<void,std::function<void(void*)>>;
std::pair<FileMap,size_t> fileMapBuf(const char* fileName);

}//namespace
