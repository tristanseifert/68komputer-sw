cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)

# compile the standard library
add_subdirectory(stdlib)

# tests
add_subdirectory(hello-world)
