// TMC_IMPL is not defined here, as it is in tmc_build.cpp

#include "tmc/all_headers.hpp"

#include <cstdio>

tmc::task<int> my_async_main() {
  std::printf("Hello, async world!\n");
  co_return 0;
}

int main() { return tmc::async_main(my_async_main()); }
