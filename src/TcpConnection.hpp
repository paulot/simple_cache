#pragma once

#include <ctime>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "Cache.hpp"


class TcpConnection
  : public boost::enable_shared_from_this<TcpConnection> {
public:
  typedef boost::shared_ptr<TcpConnection> pointer;

  static pointer create(
      boost::asio::io_service& io_service,
      const std::shared_ptr<Cache<std::string, std::string>>& cache) {
    return pointer(new TcpConnection(io_service, cache));
  }

  boost::asio::ip::tcp::socket& socket();

  void handleRead(const boost::system::error_code& error);
  void handleRead();

private:
  // constexpr static uint32_t maxLen_ = 1024;
  constexpr static uint32_t maxLen_ = 50;
  // char message_[maxLen_];
  boost::asio::streambuf message_;


  boost::asio::ip::tcp::socket socket_;
  std::shared_ptr<Cache<std::string, std::string>> cache_;

  TcpConnection(
      boost::asio::io_service& io_service,
      const std::shared_ptr<Cache<std::string, std::string>>& cache)
      : socket_(io_service), cache_(cache) {}

  void handleMessage(size_t bytesTransferred);

  void writeResult(const std::string& message);
  void handleWrite(const boost::system::error_code& error, size_t bytesTransferred) const;

};
