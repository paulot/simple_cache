#include "TcpServer.hpp"

#include <boost/bind.hpp>

void AsyncTcpServer::start_accept() {
  TcpConnection::pointer new_connection =
    TcpConnection::create(
        (boost::asio::io_context&) acceptor_.get_executor().context(),
        cache_);

  acceptor_.async_accept(new_connection->socket(),
      boost::bind(
        &AsyncTcpServer::handle_accept,
        this,
        new_connection,
        boost::asio::placeholders::error));
}

void AsyncTcpServer::handle_accept(const TcpConnection::pointer& new_connection,
        const boost::system::error_code& error) {
  new_connection->handleRead(error);
  start_accept();
}
