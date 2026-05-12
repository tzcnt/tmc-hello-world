cmake_minimum_required(VERSION 3.16)
project(tmc_recommended)

set(CMAKE_MODULE_PATH
    ${tmc_recommended_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# TooManyCooks options. Set these before add_subdirectory()/CPMAddPackage()
# so they are applied to the TooManyCooks::TooManyCooks interface target.
# Defaults are specified below.
option(TMC_USE_HWLOC "Enable hwloc topology-aware thread placement" ON)
option(TMC_USE_BOOST_ASIO "Use boost::asio instead of standalone asio" OFF)

set(TMC_WORK_ITEM "CORO" CACHE STRING "Work item type: CORO or FUNCORO")
set_property(CACHE TMC_WORK_ITEM PROPERTY STRINGS "CORO" "FUNCORO")

option(TMC_TRIVIAL_TASK "Enable trivial task optimization" OFF)
option(TMC_NODISCARD_AWAIT "Enable [[nodiscard]] on co_await" OFF)

set(TMC_PRIORITY_COUNT "" CACHE STRING "Number of priority levels (1-16, empty for runtime-configurable)")

option(TMC_MORE_THREADS "Allow more than 64 threads" OFF)
option(TMC_DEBUG_TASK_ALLOC_COUNT "Track task allocation count for HALO analysis" OFF)
option(TMC_DEBUG_THREAD_CREATION "Print thread creation and work stealing info" OFF)

option(TMC_STANDALONE_COMPILATION "Disable header-only mode. Library will be built into the file that defines TMC_IMPL" OFF)

include(cmake/CPM.cmake)
CPMAddPackage(
    NAME TooManyCooks
    GIT_REPOSITORY https://github.com/tzcnt/TooManyCooks.git
    GIT_TAG v1.5)

# #### High-performance allocators #####

# Since each new coroutine requires an allocation,
# they are sensitive to allocator performance.
# Any of tcmalloc, mimalloc, or jemalloc provide
# greatly superior performance to the default glibc malloc.
# Try to find any of these 3 before falling back to default.
find_package(libtcmalloc)

if(LIBTCMALLOC_FOUND)
    set(MALLOC_LIB "${LIBTCMALLOC_LIBRARY}")
    message(STATUS "Using malloc: ${MALLOC_LIB}")
else()
    find_package(libmimalloc)

    if(LIBMIMALLOC_FOUND)
        set(MALLOC_LIB "${LIBMIMALLOC_LIBRARY}")
        message(STATUS "Using malloc: ${MALLOC_LIB}")
    else()
        find_package(libjemalloc)

        if(LIBJEMALLOC_FOUND)
            set(MALLOC_LIB "${LIBJEMALLOC_LIBRARY}")
            message(STATUS "Using malloc: ${MALLOC_LIB}")
        else()
            message(STATUS "WARNING: No high performance allocator was found. Using default system allocator.")
        endif()
    endif()
endif()

link_libraries(${MALLOC_LIB})

# #### boost::asio or standalone asio support #####

# Tell cmake to use the target Boost cmake file instead of the built-in
cmake_policy(SET CMP0167 NEW)

# By default this will will look for your system boost.
# If this fails you can install boost manually and set the system environment variable
# BOOST_CMAKE_ROOT with the /stage/lib/cmake directory being where boost places the CMake files after building with b2.

# set(ENV{BOOST_CMAKE_ROOT} "/home/tzcnt/pkg-src/boost-1.88-src/boost_1_88_0/stage/lib/cmake/")
if(DEFINED ENV{BOOST_CMAKE_ROOT})
    find_package(
        Boost 1.82.0 CONFIG QUIET
        PATHS $ENV{BOOST_CMAKE_ROOT} NO_DEFAULT_PATH
    )
else()
    find_package(Boost 1.82.0 QUIET)
endif()

if(Boost_FOUND)
    add_compile_definitions(TMC_USE_BOOST_ASIO)

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_RELEASE_LIBS ON)

    message(STATUS "Using boost::asio from: ${Boost_DIR}")
else()
    message(STATUS "boost::asio not found. Using standalone asio.")
    CPMAddPackage(
        NAME asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        VERSION 1.36.0
        GIT_TAG asio-1-36-0
        DOWNLOAD_ONLY)
    set(ASIO_INCLUDE_PATH ${asio_SOURCE_DIR}/asio/include CACHE STRING "" FORCE)

    # mark Asio headers as SYSTEM to ignore warnings
    include_directories(SYSTEM
        ${ASIO_INCLUDE_PATH}
    )
endif()

add_executable(hello_world
    src/hello_world/main.cpp
)
target_link_libraries(hello_world PRIVATE TooManyCooks::TooManyCooks)

add_executable(hello_world_asio
    src/hello_world_asio/main.cpp
)
target_link_libraries(hello_world_asio PRIVATE TooManyCooks::TooManyCooks)
