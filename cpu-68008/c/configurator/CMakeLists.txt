project(configurator C ASM)
cmake_minimum_required(VERSION 3.2)

# link against the stdlib
add_subdirectory(../stdlib stdlib)

# create the binary
add_executable(configurator
  ${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/platform/start.S
  main.c
  ScreenMode.c
  ScreenMode.h
  TitleBar.c
  TitleBar.h
  MainScreen.c
  MainScreen.h
  StatusBar.c
  StatusBar.h
)

target_include_directories(configurator PUBLIC ../stdlib/include)
target_include_directories(configurator PUBLIC ../stdlib)
target_link_libraries(configurator c-68komputer)

# optimize for speed
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")

# ensure we use the 68komputer linker script
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/../stdlib/platform/68komputer.ld" )
# set_target_properties(hello-world PROPERTIES LINK_DEPENDS ../../stdlib/platform/68komputer.ld)
