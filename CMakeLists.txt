cmake_minimum_required (VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project (vkt)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")

find_package(PkgConfig REQUIRED)
pkg_check_modules(WL wayland-client)

add_executable(vkt
	waylandsuf.cc
	vk.cc
	vkpipe.cc
	vkbuf.cc
	main.cc
	)

target_link_libraries(vkt
	${WL_LIBRARIES}
	-lvulkan
)




