################################################################################
# compile into platform module
################################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tinyalloc)

add_library(platform OBJECT
# start.S
init.c
malloc.c
putchar.c
getchar.c
tty.c
mc68681_gpio.c
mc68681_gpio.h
unpack.c
unpack.h
)
