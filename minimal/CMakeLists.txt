cmake_minimum_required(VERSION 3.16)
project(tmc_minimal)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  TooManyCooks
  GIT_REPOSITORY https://github.com/tzcnt/TooManyCooks.git
  GIT_TAG v1.1.0
)
FetchContent_MakeAvailable(TooManyCooks)

include_directories(
    ${toomanycooks_SOURCE_DIR}/include
)

add_executable(hello_world
    src/main.cpp
)
