#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TcpConnection.hpp"

using boost::asio::ip::tcp;


class AsyncTcpServer{
 public:
  AsyncTcpServer(boost::asio::io_service& io_service, int32_t port)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
  }

private:
  void start_accept();

  void handle_accept(tcp_connection::pointer new_connection,
      const boost::system::error_code& error);

  tcp::acceptor acceptor_;
};

