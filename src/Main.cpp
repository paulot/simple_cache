#include <thread>

#include "Cache.hpp"
#include "TcpServer.hpp"

int main() {
  try {
    const int32_t capacity = 10;
    auto cache = std::make_shared<Cache<std::string, std::string>>(capacity);
    boost::asio::io_service io_service(std::thread::hardware_concurrency() * 4);
    AsyncTcpServer server(io_service, 13, std::move(cache));
    io_service.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
