#!/bin/bash

PRESET=gcc-linux-debug
# The following PRESET options are available in this project's CMakePresets.json:
#
# For Linux:
# clang-linux-debug
# clang-linux-release
# clang-linux-relwithdebinfo
# gcc-linux-debug
# gcc-linux-release
# gcc-linux-relwithdebinfo
#
# For Windows:
# clang-win-debug
# clang-win-release
# clang-win-relwithdebinfo
# msvc-win-debug
# msvc-win-release
# msvc-win-relwithdebinfo
# 
# For MacOS:
# clang-macos-debug
# clang-macos-release
# clang-macos-relwithdebinfo

cmake --preset $PRESET .
cmake --build ./build --parallel 16 --target all
