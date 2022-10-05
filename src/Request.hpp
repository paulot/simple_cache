#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>

enum RequestType {
  GET = 1,
  PUT = 2,
  CLOSE = 3,
};

class Request {
public:
  // static Request deserialize(char* buffer, size_t bytesTransferred, size_t bufferLen);
  // static Request deserialize(std::string& data);
  static Request deserialize(boost::asio::streambuf& data);
  static std::string serializeClose();
  static std::string serializeGet(const std::string& key);
  static std::string serializePut(const std::string& key, const std::string& value);

  RequestType type_;
  std::vector<std::string> params_;
  static constexpr char delim_ = ' ';
  static constexpr char term_ = '\n';
private:

  Request() : type_(RequestType::CLOSE) {}
  Request(const std::string& key) : type_(RequestType::GET), params_{key} {}
  Request(const std::string& key, const std::string& value) : type_(RequestType::PUT), params_{key, value} {}
};
