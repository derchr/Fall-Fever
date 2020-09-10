# The only purpose of this file is to add stb to the include paths
# stb.c is a wrapper file
add_library(
    stb STATIC
    stb.c
)

target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
