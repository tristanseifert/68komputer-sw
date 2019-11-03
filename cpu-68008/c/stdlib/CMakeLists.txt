project(libc-68komputer C ASM)

cmake_minimum_required(VERSION 3.2)

# include directrories
include_directories(include)

# source files
set(SOURCE_FILES
    version.h
    version.c
)

# various sub-libraries
add_subdirectory(stdlib)
add_subdirectory(string)
add_subdirectory(ctype)
add_subdirectory(platform)
add_subdirectory(math)
add_subdirectory(io)

# compile the main lib
add_library(c-68komputer SHARED STATIC ${SOURCE_FILES})

# include printf
add_library(printf OBJECT printf/printf.h printf/printf.c)
add_definitions(-DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL -DPRINTF_DISABLE_SUPPORT_LONG_LONG -DPRINTF_DISABLE_SUPPORT_PTRDIFF_T)

# and link against it the sub libraries
target_link_libraries(c-68komputer stdlib ctype string printf platform math io)
