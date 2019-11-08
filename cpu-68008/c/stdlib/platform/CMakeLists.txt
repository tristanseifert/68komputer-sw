################################################################################
# compile into platform module
################################################################################
# include_directories(umm_malloc/includes/c-helper-macros)

add_library(platform OBJECT
# start.S
umm_malloc_cfg.h
umm_malloc.c
init.c
malloc.c
putchar.c
getchar.c
tty.c
mc68681_gpio.c
mc68681_gpio.h
unpack.c
unpack.h
clock.c
clock.h
sleep.c
sleep.h
)
