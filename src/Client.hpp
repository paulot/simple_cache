#pragma once
#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include "Utils.hpp"
#include "Response.hpp"
#include "Request.hpp"

namespace cache {

class CacheClient {
public:
  CacheClient(
      boost::asio::io_service& ioService,
      const std::string& addr,
      uint32_t port) :
    addr_(addr), port_(port), socket_(ioService) {}

  void connect();
  void put(const std::string& key, const std::string& value);
  boost::optional<std::string> get(const std::string& key);

private:
  std::string addr_;
  uint32_t port_;
  boost::asio::ip::tcp::socket socket_;

  void write(const std::string& msg);
  Response read();

};

void CacheClient::connect() {
  socket_.connect(
      boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string(addr_), port_));
}

void CacheClient::write(const std::string& msg) {
  boost::system::error_code error;
  boost::asio::write(socket_, boost::asio::buffer(msg), error);

  if (error) {
    throw std::runtime_error(error.message());
  }
}

Response CacheClient::read() {
  boost::system::error_code error;
  boost::asio::streambuf buf;
  boost::asio::read_until(
      socket_,
      buf,
      Response::term_,
      error);
  
  if (error) {
    throw std::runtime_error(error.message());
  }

  return Response::deserialize(buf);
}

void CacheClient::put(const std::string& key, const std::string& value) {
  std::string serializedRequest = Request::serializePut(key, value);
  write(serializedRequest);
  auto response = read();

  if (response.error_) {
    assert(response.value_ != boost::none);
    throw std::runtime_error(*response.value_);
  }
}

boost::optional<std::string> CacheClient::get(const std::string& key) {
  std::string serializedRequest = Request::serializeGet(key);
  write(serializedRequest);
  auto response = read();

  if (response.error_) {
    assert(response.value_ != boost::none);
    throw std::runtime_error(*response.value_);
  }

  if (response.value_ == boost::none) {
    return boost::none;
  } else {
    // Copy the data from the buffer to the output string
    return *response.value_;
  }
}

} // namespace cache

