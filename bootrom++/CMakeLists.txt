####################################################################################################
# Build script for the 68komputer boot ROM
#
# The ROM consists of a few assembly stubs to perform early boot initialization, and to handle the
# interrupts, but most of it is written in C++. It is intended to be cross-compiled using LLVM 13
# or later.
####################################################################################################
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(68komputer-bootrom VERSION 0.1 LANGUAGES C CXX ASM)

### Force using VASM for the assembly files
set(CMAKE_ASM_COMPILER "vasmm68k_mot")
SET(CMAKE_ASM_COMPILE_OBJECT
    "<CMAKE_ASM_COMPILER> -Felf -m68000 -esc -spaces -o <OBJECT> <SOURCE>"
)

### use lld for linking, rather than ld
### NOTE: This requires patches to lld for 68k support
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

### required modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(ExternalProject)
include(FetchContent)
include(GenerateExportHeader)

### Force C and C++ standards, warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wmissing-declarations -Wformat=2 -Wcast-qual
        -fdiagnostics-color=always -Wwrite-strings -Wimplicit-fallthrough
        -Wno-unused-private-field -Wno-undef -Wno-unused-command-line-argument
        -Wno-unused-but-set-variable)
    add_compile_options(-Werror)
else()
    message(FATAL_ERROR "We do not know what C/C++ warning flags to use for compiler '${CMAKE_CXX_COMPILER_ID}'!")
endif()

# build the ROM
add_executable(bootrom
    src/entry.asm
    src/start.cpp
    src/vector/table.cpp
    src/vector/ll_handlers.asm
    src/io/Console.cpp
    src/hw/init.cpp
    src/hw/Xr68c681.cpp
)
target_include_directories(bootrom PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

target_compile_options(bootrom PRIVATE -fno-rtti -ffreestanding -mcpu=M68000)
target_link_options(bootrom PRIVATE -mcpu=M68000)
target_link_options(bootrom PRIVATE -static -nostdlib -ffreestanding)

# we have to use a custom linker script
target_link_options(bootrom PRIVATE "-Wl,-T${CMAKE_CURRENT_LIST_DIR}/bootrom.ld")
set_target_properties(bootrom PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/bootrom.ld)

# after building, extract the .text section and save it as a raw binary
find_program(CMAKE_OBJCOPY objcopy)
add_custom_command(TARGET bootrom POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary --only-section=.text $<TARGET_FILE:bootrom> ${CMAKE_CURRENT_BINARY_DIR}/bootrom.bin)
