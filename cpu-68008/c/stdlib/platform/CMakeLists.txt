################################################################################
# compile into platform module
################################################################################
add_library(platform OBJECT
# start.S
putchar.c
getchar.c

mc68681_gpio.c
mc68681_gpio.h
)
