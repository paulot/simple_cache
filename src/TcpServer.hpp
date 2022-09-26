#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TcpConnection.hpp"


/**
 * Simple async TCP server class leveraging boost async io.
 */
class AsyncTcpServer{
 public:
  AsyncTcpServer(boost::asio::io_service& io_service, int32_t port)
    : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    start_accept();
  }

private:
  void start_accept();

  void handle_accept(TcpConnection::pointer new_connection,
      const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
};

