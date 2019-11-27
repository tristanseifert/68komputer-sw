################################################################################
# compile into math module
################################################################################
add_library(math OBJECT
udivhi3.c
udivmod.c
udivmodhi4.c
udivmodsi4.c
divmod.c
_mulsi3.c
_udivmodsi4.c
_udivsi3.c
_umodsi3.c
)
