// A simple "Hello, World!" HTTP response server
// Listens on http://localhost:55550/

#ifdef _WIN32
#include <sdkddkver.h>
#endif

// TMC_IMPL is not defined here, as it is in tmc_build.cpp

#include "tmc/asio/aw_asio.hpp"
#include "tmc/asio/ex_asio.hpp"
#include "tmc/ex_cpu.hpp"
#include "tmc/fork_group.hpp"
#include "tmc/task.hpp"

#ifdef TMC_USE_BOOST_ASIO
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

namespace asio = boost::asio;
using boost::system::error_code;
#else
#include <asio/buffer.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

using asio::error_code;
#endif

#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>

using asio::ip::tcp;

const std::string static_response = R"(HTTP/1.1 200 OK
Content-Length: 12
Content-Type: text/plain; charset=utf-8

Hello World!)";

tmc::task<void> handler(auto Socket) {
  char data[4096];
  while (Socket.is_open()) {
    auto d = asio::buffer(data);
    auto [error, n] = co_await Socket.async_read_some(d, tmc::aw_asio);
    if (error) {
      Socket.close();
      co_return;
    }

    auto d2 = asio::buffer(static_response);
    auto [error2, n2] = co_await asio::async_write(Socket, d2, tmc::aw_asio);
    if (error2) {
      Socket.close();
      co_return;
    }
  }
  Socket.shutdown(tcp::socket::shutdown_both);
  Socket.close();
}

static tmc::task<void> accept(uint16_t Port) {
  tcp::acceptor acceptor(tmc::asio_executor(), {tcp::v4(), Port});
  auto handlers = tmc::fork_group();
  while (true) {
    auto [error, sock] = co_await acceptor.async_accept(tmc::aw_asio);
    if (error) {
      break;
    }
    handlers.fork(handler(std::move(sock)));
  }
  // Wait for all running handlers to complete.
  co_await std::move(handlers);
}

int main() {
  tmc::cpu_executor().init();
  tmc::asio_executor().init();
  return tmc::async_main([]() -> tmc::task<int> {
    std::printf("HTTP server started. Open http://localhost:55550/ in your "
                "browser.\n");
    std::printf("To exit this application, press Ctrl-C.\n");

    co_await accept(55550);
    co_return 0;
  }());
}
