#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/optional.hpp>


enum ReturnCode {
  OK = 0,
  ERROR = 1
};

class Response {
public:
  static Response deserialize(boost::asio::streambuf& data);
  static std::string serializeOk(); 
  static std::string serializeOk(const std::string& value); 
  static std::string serializeOk(const boost::optional<std::string>& maybeValue); 
  static std::string serializeError(const std::string& error);

  bool error_;
  boost::optional<std::string> value_;
  static constexpr char delim_ = ' ';
  static constexpr char term_ = '\n';
private:
  Response(bool error) : error_(error), value_(boost::none) {}
  Response(bool error, const std::string& value) : error_(error), value_(value) {}
};
