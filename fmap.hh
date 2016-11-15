#pragma once
#include <iostream>
#include <experimental/filesystem>

#include <memory>


namespace tt{
std::unique_ptr<void,std::function<void(void*)>> fileMapBuf(const char* fileName,size_t* fileSize = nullptr);

}//namespace
