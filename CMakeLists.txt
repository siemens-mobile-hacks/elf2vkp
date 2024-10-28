cmake_minimum_required(VERSION 3.9)

project(elf2vkp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories("./third_party/argparse/include" "./third_party")

add_executable(elf2vkp src/main.cpp)
target_precompile_headers(elf2vkp PRIVATE <argparse/argparse.hpp> <string> <vector>)
# target_link_options(elf2vkp PUBLIC -static)
target_compile_options(elf2vkp PUBLIC -O3)
install(TARGETS elf2vkp DESTINATION /usr/bin)
