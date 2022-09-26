#include "TcpConnection.hpp"

tcp::socket& TcpConnection::socket() {
    return this->socket_;
}

void TcpConnection::start() {
    // message_ = make_daytime_string();
    time_t now = std::time(0);
    message_ = std::ctime(&now);

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&TcpConnection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}
