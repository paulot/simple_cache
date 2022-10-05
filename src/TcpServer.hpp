#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "Cache.hpp"
#include "TcpConnection.hpp"


/**
 * Simple async TCP server class leveraging boost async io.
 */
class AsyncTcpServer{
public:
  AsyncTcpServer(
      boost::asio::io_service& io_service,
      int32_t port,
      std::shared_ptr<Cache<std::string, std::string>>&& cache)
    : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      cache_(cache) {
    this->start_accept();
  }

private:
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<Cache<std::string, std::string>> cache_;

  void start_accept();

  void handle_accept(const TcpConnection::pointer& new_connection,
      const boost::system::error_code& error);
};

