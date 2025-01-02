cmake_minimum_required(VERSION 3.9)

project(elf2vkp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories("./third_party/argparse/include" "./third_party")

add_executable(elf2vkp src/main.cpp)

target_precompile_headers(elf2vkp PRIVATE <argparse/argparse.hpp> <string> <vector>)

# EXL, 02-Jan-2025: Enable high levels of compiler warnings.
target_compile_options(elf2vkp
	PUBLIC -O3
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W3>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# target_link_options(elf2vkp PUBLIC -static)
install(TARGETS elf2vkp DESTINATION /usr/bin)
