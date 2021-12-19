####################################################################################################
# Build external components
####################################################################################################

add_library(printf STATIC
    printf/printf.c
)
target_compile_definitions(printf PRIVATE -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL -DPRINTF_DISABLE_SUPPORT_LONG_LONG -DPRINTF_DISABLE_SUPPORT_PTRDIFF_T)
target_include_directories(printf PUBLIC printf)
