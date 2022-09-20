#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;

/*
std::string make_daytime_string()
{
  time_t now = std::time(0);
  return std::ctime(&now);
}
*/

class TcpConnection
  : public boost::enable_shared_from_this<TcpConnection> {
public:
  typedef boost::shared_ptr<TcpConnection> pointer;

  static pointer create(boost::asio::io_service& io_service) {
    return pointer(new TcpConnection(io_service));
  }

  tcp::socket& socket();

  void start();

private:
  tcp::socket socket_;
  std::string message_;

  TcpConnection(boost::asio::io_service& io_service)
      : socket_(io_service) {}

  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/) {}
};
