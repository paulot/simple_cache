#include "TcpServer.hpp"

int main() {
  try {
    boost::asio::io_service io_service;
    AsyncTcpServer server(io_service, 13);
    io_service.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
