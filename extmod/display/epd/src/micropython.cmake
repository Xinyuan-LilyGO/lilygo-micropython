# Create an INTERFACE library for our C module.
add_library(usermod_epd INTERFACE)

set(EPD47_DIR ${CMAKE_BINARY_DIR}/../LilyGo-EPD47)

file(GLOB LIBEPD47_SRC ${EPD47_DIR}/src/*.c)
file(GLOB ZLIB_SRC ${EPD47_DIR}/src/zlib/*.c)
file(GLOB LIBJPEG_SRC ${EPD47_DIR}/src/libjpeg/*.c)

# Add our source files to the lib
target_sources(usermod_epd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modepd.c
    ${CMAKE_CURRENT_LIST_DIR}/epd_epd47.c
    ${LIBEPD47_SRC}
    ${ZLIB_SRC}
    ${LIBJPEG_SRC}
)

# Add the current directory as an include directory.
target_include_directories(usermod_epd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${EPD47_DIR}/src/
    ${EPD47_DIR}/src/zlib/
    ${EPD47_DIR}/src/libjpeg/
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_epd)
