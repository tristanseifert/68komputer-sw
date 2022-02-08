# build the Enhanced BASIC ROM
add_custom_command(OUTPUT romapp_basic.c
    # assemble it
    # we disable warning 2028 (using signed operand as unsigned, moveq overflow) since the ehbasic
    # code uses it in a few places, seemingly intentionally
    COMMAND vasmm68k_mot -Fbin -m68000 -esc -spaces -nowarn=2028 -L ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.lst -o ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin ${CMAKE_CURRENT_SOURCE_DIR}/src/ehbasic.asm
    # compress it
    COMMAND ${LZ4_PROGRAM} -z -12 -f --no-frame-crc ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin.lz4
    # convert to header
    COMMAND ${Python3_EXECUTABLE} ${BIN2HDR_PATH} --const ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin.lz4 romapp_basic
    # rebuild when basic source or tool source changes
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/ehbasic.asm ${BIN2HDR_PATH}
    # secrete raw and compressed binary
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin.lz4 romapp_basic.h
    COMMENT "Building Enhanced BASIC"
    VERBATIM
)

add_library(RomAppBasic STATIC
    romapp_basic.c
)
target_include_directories(RomAppBasic PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

