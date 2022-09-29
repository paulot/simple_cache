#include "TcpConnection.hpp"

tcp::socket& TcpConnection::socket() {
    return this->socket_;
}


  /*
    // message_ = make_daytime_string();
    time_t now = std::time(0);
    message_ = std::ctime(&now);

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        boost::bind(&TcpConnection::handleWrite, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  */

void TcpConnection::handleRead(const boost::system::error_code& error) {
  if (!error) {
    std::cout << message_ << std::endl;
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(message_, TcpConnection::maxLen_),
        boost::bind(
          &TcpConnection::handleRead,
          shared_from_this(),
          boost::asio::placeholders::error));
  }
}



/**
  boost::asio::streambuf streambuf;
  boost::asio::async_read_until(socket_, streambuf, delimiter_,
    [this, &streambuf](
      const boost::system::error_code& error_code,
      size_t bytes_transferred) {

      std::cout << "reading from socket bytes_transfered " << bytes_transferred << std::endl;
      // Verify streambuf contains more data beyond the delimiter. (e.g.
      // async_read_until read beyond the delimiter)
      // assert(streambuf.size() > bytes_transferred);

      if (bytes_transferred> 0) {
        // Extract up to the first delimiter.
//         std::string command{
//           buffers_begin(streambuf.data()),
//           buffers_begin(streambuf.data()) + bytes_transferred
//             - delimiter_.size()};
//         std::string command{
//           buffers_begin(streambuf.data()),
//           buffers_begin(streambuf.data()) + bytes_transferred};


        // Consume through the first delimiter so that subsequent async_read_until
        // will not reiterate over the same data.
        streambuf.consume(bytes_transferred);

        // std::cout << "received command: " << command << "\n"
        std::cout << "streambuf contains " << streambuf.size() << " bytes."
                  << std::endl;

      }
    }
  );
 *

boost::asio::streambuf streambuf;
  boost::asio::async_read_until(socket2, streambuf, delimiter,
    [delimiter, &streambuf](
      const boost::system::error_code& error_code,
      std::size_t bytes_transferred)
    {
      // Verify streambuf contains more data beyond the delimiter. (e.g.
      // async_read_until read beyond the delimiter)
      assert(streambuf.size() > bytes_transferred);

      // Extract up to the first delimiter.
      std::string command{
        buffers_begin(streambuf.data()),
        buffers_begin(streambuf.data()) + bytes_transferred
          - delimiter.size()};

      // Consume through the first delimiter so that subsequent async_read_until
      // will not reiterate over the same data.
      streambuf.consume(bytes_transferred);

      assert(command == "cmd1");
      std::cout << "received command: " << command << "\n"
                << "streambuf contains " << streambuf.size() << " bytes."
                << std::endl;
    }
  );
*/
