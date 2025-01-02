cmake_minimum_required(VERSION 3.9)

project(elf2vkp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories("./third_party/argparse/include" "./third_party")

add_executable(elf2vkp src/main.cpp)

target_precompile_headers(elf2vkp PRIVATE <argparse/argparse.hpp> <string> <vector>)

# EXL, 02-Jan-2025: Enable high levels of compiler warnings and proper optimizations flags.
target_compile_options(elf2vkp
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:/O2>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3>
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W3>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# EXL, 02-Jan-2025: Enable static linking and executable stripping for MinGW.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_link_options(elf2vkp PUBLIC
		"-s"
		"-static"
		"-static-libgcc"
		"-static-libstdc++"
		"-Wl,--gc-sections"
	)
endif()

install(TARGETS elf2vkp DESTINATION /usr/bin)
