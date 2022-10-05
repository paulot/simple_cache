#include <istream>
#include <boost/tokenizer.hpp>

#include "Response.hpp"
#include "Utils.hpp"


Response Response::deserialize(boost::asio::streambuf& data) {
  std::istream input(&data);
  std::string token;
  std::getline(input, token, delim_);
  int32_t returnCode = std::stoi(token);

  if (token.size() == 0) {
    throw std::runtime_error("Expected to deserialize error code, got nothing.");
  }

  if (input.eof()) {
    // No value or error message present
    return Response((ReturnCode) returnCode);
  }

  // Value or error message present
  std::getline(input, token, term_);
  return Response((ReturnCode) returnCode, token);
}

std::string Response::serializeOk() {
  return utils::toString(ReturnCode::OK, term_);
}

std::string Response::serializeOk(const std::string& value) {
  return utils::toString(ReturnCode::OK, delim_, value, term_);
}

std::string Response::serializeOk(const boost::optional<std::string>& maybeValue) {
  if (maybeValue != boost::none) {
    return serializeOk(*maybeValue);
  } else {
    return serializeOk();
  }
}

std::string Response::serializeError(const std::string& error) {
  return utils::toString(ReturnCode::ERROR, delim_, error, term_);
}



/*
  char *token{std::strtok(boost::asio::buffer_cast<char*>(data.data()), delim_)};
  int32_t returnCode = std::atoi(token);


  char *value = std::strtok(NULL, "");

  if (value == NULL) {
    // No value or error message present
    return Response((ReturnCode) returnCode);
  } else {
    // Value or error message present
    return Response((ReturnCode) returnCode, value);
  }
*/

  /*
  // buffer[std::min<size_t>(bufferLen - 1, bytesTransferred)] = '\0';
  boost::char_separator<char> sep{" "};
  boost::tokenizer<boost::char_separator<char>> tok{data, sep};
  auto it = tok.begin();
  int32_t returnCode = std::stoi(*it);
  
  if (returnCode != 0 && returnCode != 1) {
    throw std::runtime_error(utils::toString("Unknown return code: ", returnCode));
  }

  if ((*it).size() == data.size()) {
    // No value or error message present
    return Response((ReturnCode) returnCode);
  } else {
    // Value or error message present
    return Response((ReturnCode) returnCode, data.substr((*it).size()));
  }
  */
