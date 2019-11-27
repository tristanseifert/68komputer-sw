project(gpio-test C ASM)
cmake_minimum_required(VERSION 3.2)

# link against the stdlib
add_subdirectory(../../../stdlib stdlib)

# create the binary
add_executable(gpio-test
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../stdlib/platform/start.S
  main.c
)

target_include_directories(gpio-test PUBLIC ../../../stdlib/include)
target_include_directories(gpio-test PUBLIC ../../../stdlib)
target_link_libraries(gpio-test c-68komputer)

# ensure we use the 68komputer linker script
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/../../../stdlib/platform/68komputer.ld")
# set_target_properties(hello-world PROPERTIES LINK_DEPENDS ../../stdlib/platform/68komputer.ld)
