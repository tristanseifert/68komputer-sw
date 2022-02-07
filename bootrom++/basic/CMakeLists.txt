# build the Enhanced BASIC ROM
add_custom_target(ehbasic
    COMMAND vasmm68k_mot -Fbin -m68000 -esc -spaces -o ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin ${CMAKE_CURRENT_SOURCE_DIR}/src/ehbasic.asm
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/ehbasic.asm
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin
    COMMENT "Building Enhanced BASIC"
)

# compress the BASIC ROM
add_custom_command(TARGET ehbasic POST_BUILD
    COMMAND ${LZ4_PROGRAM} -z -12 -f --no-frame-crc ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin ${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin.lz4
    COMMENT "Compressing Enhanced BASIC"
)

# convert the binary to a C header
set(INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/ehbasic.bin.lz4")
set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/romapp_basic.h")

add_custom_command(TARGET ehbasic POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND ${Python3_EXECUTABLE} ${BIN2HDR_PATH} --const ${INPUT_FILE} ${OUTPUT_FILE}
    DEPENDS ${BIN2HDR_PATH} ${INPUT_FILE}
    COMMENT "Creating C header for Enhanced BASIC")

target_sources(RomApps INTERFACE ${OUTPUT_FILE})
target_include_directories(RomApps INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(RomApps ehbasic ${INPUT_FILE} ${OUTPUT_FILE})
