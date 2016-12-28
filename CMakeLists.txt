cmake_minimum_required (VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project (vkt)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "-Wall -std=c++1z")

find_package(PkgConfig REQUIRED)
pkg_check_modules(WL wayland-client)

add_executable(vkt
	waylandsuf.cc
	vk.cc
	vkpipe.cc
	vkbuf.cc
	fmap.cc
	main.cc
)

target_link_libraries(vkt
	${WL_LIBRARIES}
	-lvulkan
	-lstdc++fs
)


set(GLSL_VALIDATOR glslangValidator)

set(GLSL_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/shaders/tri.frag
	${PROJECT_SOURCE_DIR}/shaders/tri.vert
)

foreach(GLSL ${GLSL_SOURCE_FILES})
	get_filename_component(FILE_NAME ${GLSL} NAME)
	set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
		COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
	DEPENDS ${GLSL})
	list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(vkt Shaders)


