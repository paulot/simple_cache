#include "TcpServer.hpp"


void AsyncTcpServer::start_accept() {
  tcp_connection::pointer new_connection =
    tcp_connection::create((boost::asio::io_context&) acceptor_.get_executor().context());

  acceptor_.async_accept(new_connection->socket(),
      boost::bind(&AsyncTcpServer::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
}

void AsyncTcpServer::handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& error) {
  if (!error) {
    new_connection->start();
  }

  // start_accept();
}
