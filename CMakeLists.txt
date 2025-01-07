cmake_minimum_required(VERSION 3.10)

project(elf2vkp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories("./third_party/argparse/include" "./third_party")
add_executable(elf2vkp src/main.cpp)
target_precompile_headers(elf2vkp PRIVATE <argparse/argparse.hpp> <string> <vector>)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
	if (BUILD_STATIC)
		target_link_options(elf2vkp PUBLIC -static -static-libgcc -static-libstdc++)
	endif()

	if (CMAKE_BUILD_TYPE MATCHES "Release")
		target_link_options(elf2vkp PUBLIC -s)
	endif()

	target_compile_options(elf2vkp PUBLIC -Wall -Wextra -O2)
	install(TARGETS elf2vkp DESTINATION /usr/bin)
endif()
