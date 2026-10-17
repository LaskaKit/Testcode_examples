# This is the project CMake build system file for the ESP-IDF build system.
# To build the project and flash it to the board, use:
#
#     idf.py -p PORT build flash
#
# You can change the build type and verbosity using:
#
#     idf.py -p PORT -b debug build
#
# If you don't specify -p PORT, esptool.py will try to auto-detect the port.

cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(GDEH037E01_EPD)
