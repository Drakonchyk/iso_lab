cmake_minimum_required(VERSION 3.10)

project(CreateISO VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(create_iso main.cpp)

target_compile_options(create_iso PRIVATE -Wall -Wextra -pedantic)

set_target_properties(create_iso PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
