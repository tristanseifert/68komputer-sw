################################################################################
# compile into platform module
################################################################################
add_library(platform OBJECT
# start.S
putchar.c
getchar.c
tty.c
mc68681_gpio.c
mc68681_gpio.h
)
