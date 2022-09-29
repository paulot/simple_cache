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

  void handleRead(const boost::system::error_code& error);

private:
  const std::string delimiter_ = "\n";
  constexpr static uint32_t maxLen_ = 1024;
  tcp::socket socket_;
  char message_[maxLen_];

  TcpConnection(boost::asio::io_service& io_service)
      : socket_(io_service) {}

  void handleWrite(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/) {}

};
