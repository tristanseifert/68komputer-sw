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
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wmissing-declarations -Wformat=2 -Wcast-qual
        -fdiagnostics-color=always -Wwrite-strings -Wimplicit-fallthrough
        -Wno-undef -Wno-unused-but-set-variable -Wno-unknown-pragmas -Wno-multichar)
    add_compile_options(-Werror)
else()
    message(FATAL_ERROR "We do not know what C/C++ warning flags to use for compiler '${CMAKE_CXX_COMPILER_ID}'!")
endif()

# use the freestanding baremetal stuff
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-ffreestanding -mcpu=M68000)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-ffreestanding -mcpu=68000)
else()
    message(FATAL_ERROR "We do not know what C/C++ flags to use for compiler '${CMAKE_CXX_COMPILER_ID}'!")
endif()

# get the Git version and generate the version header
find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty=-dirty --always OUTPUT_VARIABLE VERSION_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD OUTPUT_VARIABLE VERSION_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file(${CMAKE_CURRENT_LIST_DIR}/src/runtime/version.c.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.c)

# external libraries
add_subdirectory(external)

# external components
add_library(runtime STATIC
    src/runtime/math/_mulsi3.c
    src/runtime/math/_udivmodsi4.c
    src/runtime/math/_udivsi3.c
    src/runtime/math/_umodsi3.c
    src/runtime/string/memchr.c
    src/runtime/string/memcmp.c
    src/runtime/string/memcpy.c
    src/runtime/string/memmem.c
    src/runtime/string/memmove.c
    src/runtime/string/memset.c
    src/runtime/string/strchr.c
    src/runtime/string/strncat.c
    src/runtime/string/strncmp.c
    src/runtime/string/strncpy.c
    src/runtime/string/strnlen.c
    src/runtime/string/strtok.c
    src/runtime/stdlib/strtol.c
    src/runtime/stdlib/strtoul.c
    src/runtime/ctype/isalnum.c
    src/runtime/ctype/isalpha.c
    src/runtime/ctype/isascii.c
    src/runtime/ctype/isblank.c
    src/runtime/ctype/iscntrl.c
    src/runtime/ctype/isdigit.c
    src/runtime/ctype/isgraph.c
    src/runtime/ctype/islower.c
    src/runtime/ctype/isprint.c
    src/runtime/ctype/ispunct.c
    src/runtime/ctype/isspace.c
    src/runtime/ctype/isupper.c
    src/runtime/ctype/isxdigit.c
    src/runtime/ctype/toascii.c
    src/runtime/ctype/toupper.c
    src/runtime/ctype/tolower.c
    src/runtime/cppsupport.cpp
)
target_include_directories(runtime PRIVATE SYSTEM ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_options(runtime PRIVATE -Wno-missing-declarations -Wno-cast-qual)

# build the ROM
add_executable(bootrom
    src/entry.asm
    src/start.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.c
    src/vector/table.cpp
    src/vector/ll_handlers.asm
    src/io/UnitTable.cpp
    src/io/Console.cpp
    src/io/XModemReceiver.cpp
    src/svc/SvcTable.cpp
    src/hw/init.cpp
    src/hw/xr68c681/Xr68c681.cpp
    src/hw/xr68c681/isr.asm
    src/shell/Shell.cpp
    src/shell/CommandList.cpp
    src/shell/Download.cpp
    src/shell/HexDump.cpp
    src/shell/Inventory.cpp
    src/shell/Memory.cpp
    src/shell/Execute.cpp
    src/shell/Execute.asm
)
target_include_directories(bootrom PRIVATE SYSTEM ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(bootrom PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(bootrom PRIVATE printf runtime)
target_compile_options(bootrom PRIVATE -fno-exceptions)
target_link_options(bootrom PRIVATE -static -nostdlib -nostartfiles)

# we have to use a custom linker script
target_link_options(bootrom PRIVATE "-T${CMAKE_CURRENT_LIST_DIR}/bootrom.ld")
set_target_properties(bootrom PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/bootrom.ld)

# after building, extract the .text section and save it as a raw binary; and print the sizes
find_program(CMAKE_SIZE size)
find_program(CMAKE_OBJCOPY objcopy)

add_custom_command(TARGET bootrom POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:bootrom>
)
add_custom_command(TARGET bootrom POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary --only-section=.text --only-section=.data $<TARGET_FILE:bootrom> ${CMAKE_CURRENT_BINARY_DIR}/bootrom.bin)
