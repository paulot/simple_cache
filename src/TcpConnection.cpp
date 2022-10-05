#include <algorithm>
#include <cstdio>
#include <iostream>
#include <boost/bind.hpp>

#include "TcpConnection.hpp"
#include "Utils.hpp"
#include "Response.hpp"
#include "Request.hpp"


using boost::asio::ip::tcp;

tcp::socket& TcpConnection::socket() {
  return this->socket_;
}

void TcpConnection::handleRead() {
  boost::asio::async_read_until(
      socket_,
      // boost::asio::buffer(message_, TcpConnection::maxLen_),
      message_,
      Request::term_,
      boost::bind(
        &TcpConnection::handleMessage,
        shared_from_this(),
        boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::handleRead(const boost::system::error_code& error) {
  if (!error) {
    handleRead();
  }
}

void TcpConnection::writeResult(const std::string& message) {
  boost::asio::async_write(
      socket_,
      boost::asio::buffer(message, message.size()),
      boost::bind(
        &TcpConnection::handleWrite,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::handleWrite(const boost::system::error_code& error, size_t bytesTransferred) const {
  if (error) {
    std::cerr << "Error: " <<  error << std::endl;
    std::cerr << "Bytes Transferred: " << bytesTransferred << std::endl;
  }
}

void TcpConnection::handleMessage(size_t bytesTransferred) {
  try {
    auto request = Request::deserialize(message_);
  //return Response::deserialize(boost::asio::buffer_cast<const char*>(buf.data()));

    if (request.type_ == RequestType::GET) {
      assert(request.params_.size() == 1);
      auto val = cache_->get(request.params_[0]);
      writeResult(Response::serializeOk(val));
      handleRead();
    } else if (request.type_ == RequestType::PUT) {
      assert(request.params_.size() == 2);
      cache_->put(request.params_[0], request.params_[1]);
      writeResult(Response::serializeOk());
      handleRead();
    } else if (request.type_ == RequestType::CLOSE) {
      writeResult(Response::serializeOk());
    }
  } catch (const std::runtime_error& e) {
    writeResult(Response::serializeError(e.what()));
  }
}
