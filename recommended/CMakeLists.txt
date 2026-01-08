cmake_minimum_required(VERSION 3.16)
project(tmc_recommended)

set(CMAKE_MODULE_PATH
    ${tmc_recommended_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME TooManyCooks
    GIT_REPOSITORY https://github.com/tzcnt/TooManyCooks.git
    GIT_TAG v1.3.0
    DOWNLOAD_ONLY)

CPMAddPackage(
    NAME tmc_asio
    GIT_REPOSITORY https://github.com/tzcnt/tmc-asio.git
    GIT_TAG v1.3.0
    DOWNLOAD_ONLY)

include_directories(
    ${TooManyCooks_SOURCE_DIR}/include
    ${tmc_asio_SOURCE_DIR}/include
)

##### TMC HWLOC support #####

message(STATUS "running find_package(libhwloc)...")
find_package(libhwloc)

if(NOT LIBHWLOC_FOUND)
    message(STATUS "WARNING: Package libhwloc not found. hwloc integration will be disabled.")
else()
    message(STATUS " found library ${LIBHWLOC_LIBRARY}")
    message(STATUS " found include ${LIBHWLOC_INCLUDE_DIR}/hwloc.h")
    message(STATUS " hwloc integration is enabled")
    add_compile_definitions(TMC_USE_HWLOC)
    link_libraries(${LIBHWLOC_LIBRARY})
    # mark hwloc headers as SYSTEM to ignore warnings
    include_directories(SYSTEM ${LIBHWLOC_INCLUDE_DIR})
endif()


##### High-performance allocators #####

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

##### boost::asio or standalone asio support #####

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
if (Boost_FOUND)
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

##### TMC build-time options #####

add_definitions(

    # Performance tuning options
    # "-DTMC_WORK_ITEM=CORO" # one of: CORO, FUNC, FUNCORO
    # "-DTMC_TRIVIAL_TASK" # enabled in this repo for Release builds via CMakePresets.json
    # "-DTMC_PRIORITY_COUNT=1" # unsigned integer between 1 and 63

    # Debug / diagnostics options
    # tracks the number of tmc::task allocations which lets you determine
    # how effectively Heap Allocation Elision (HALO) is being applied by the compiler
    # "-DTMC_DEBUG_TASK_ALLOC_COUNT"
)

add_executable(hello_world
    src/tmc_build.cpp
    src/hello_world/main.cpp
)

add_executable(hello_world_asio
    src/tmc_build.cpp
    src/hello_world_asio/main.cpp
)
