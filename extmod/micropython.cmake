# This top-level micropython.cmake is responsible for listing
# the individual modules we want to include.
# Paths are absolute, and ${CMAKE_CURRENT_LIST_DIR} can be
# used to prefix subdirectories.

if(EXTMOD_EPD)
include(${CMAKE_CURRENT_LIST_DIR}/display/epd/src/micropython.cmake)
endif()

if(EXTMOD_GSM)
include(${CMAKE_CURRENT_LIST_DIR}/network/gsm/src/micropython.cmake)
endif()

if(EXTMOD_ST7789)
include(${CMAKE_CURRENT_LIST_DIR}/display/st7789/src/micropython.cmake)
endif()