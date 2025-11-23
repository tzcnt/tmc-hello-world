## tmc-hello-world
CMake configurations to create minimal "Hello, world!" applications
using the TooManyCooks async runtime. TMC is a header-only library and does not require or use CMake directly. CMake is used here only for example, but TMC is compatible with any build system.

There are two separate project templates here: `minimal` and `recommended`.

### `minimal` subfolder
The absolute bare minimum necessary to build and run a TMC application:
1. Download TMC using CMake FetchContent.
2. `#define TMC_IMPL` in `main.cpp`

```bash
cd ./minimal
./build.sh
./build/hello_world
```

### `recommended` subfolder
Adds the following additional elements:
1. Standalone TMC build file `tmc_build.cpp`.
2. Use [CPM](https://github.com/cpm-cmake/CPM.cmake) to download dependencies.
3. (optional) attempt to find system `libhwloc`. If found, enable `TMC_USE_HWLOC`.
4. (optional) attempt to find a system `tcmalloc`, `mimalloc`, or `jemalloc`. If found, link to it.
5. Attempt to find system boost::asio. Otherwise, fallback to fetching standalone Asio.
6. Additional example `hello_world_asio` which returns `Hello, World!` over HTTP.
7. `CMakePresets.json` which includes presets for Linux/Windows/Mac and Clang/GCC/MSVC.

```bash
cd ./recommended
# The default preset is 'gcc-linux-debug'. Edit build.sh to change it.
./build.sh
./build/hello_world
./build/hello_world_asio
```

### Dependencies
- cmake
- a C++ compiler
- (optional) `libhwloc-dev` and `libtcmalloc-minimal4` system packages
- (really optional) `libboost-all-dev` system package
