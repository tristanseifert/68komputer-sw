################################################################################
# compile all string files into the std-string object
################################################################################
add_library(stdlib OBJECT
abs.c
atoi.c
atol.c
atoll.c
bsearch.c
calloc.c
div.c
heapsort_r.c
heapsort.c
imaxabs.c
imaxdiv.c
labs.c
ldiv.c
# llabs.c
# lldiv.c
qsort_r.c
qsort.c
rand.c
realloc.c
# strtol.c
# strtoll.c
# strtoul.c
# strtoull.c
)
