#include <istream>

#include "Request.hpp"
#include "Utils.hpp"


Request Request::deserialize(boost::asio::streambuf& data) {
  std::istream input(&data);
  std::string token;
  std::getline(input, token, delim_);

  if (token == "GET" || token == "get") {
    // Get the key

    if (input.eof()) {
      throw std::runtime_error("Expected key for command, got nothing.");
    }

    std::getline(input, token, term_);
    return Request(token); 
  } else if (token == "PUT" || token == "put") {
    // Get the key

    if (input.eof()) {
      throw std::runtime_error("Expected key for command, got nothing.");
    }

    std::getline(input, token, delim_);

    if (input.eof()) {
      throw std::runtime_error("Expected value for command, got nothing.");
    }
    std::string value;
    std::getline(input, value, term_);
    return Request(token, value);
  } else if (token.size() == 0 || token == "CLOSE\n" || token == "close\n") {
    return Request();
  } else {
    throw std::runtime_error(utils::toString("Unknown command: ", token));
  }
}

std::string Request::serializeGet(const std::string& key) {
  return utils::toString("GET", delim_, key, term_);
}

std::string Request::serializePut(const std::string& key, const std::string& value) {
  return utils::toString("PUT", delim_, key, delim_, value, term_);
}

std::string Request::serializeClose() {
  return utils::toString("CLOSE", term_);
}


/*
  // buffer[std::min<size_t>(bufferLen - 1, bytesTranferred)] = '\0';
  char *token{std::strtok(boost::asio::buffer_cast<char*>(data.data()), delim_)};

  if (std::strncmp(token, "GET", 3) == 0 || std::strncmp(token, "get", 3) == 0) {
    // Do GET command
    token = std::strtok(NULL, delim_);
    
    if (token == NULL) {
      throw std::runtime_error("GET Error: Expected key, got NULL");
    }

    return Request(token);
  } else if (std::strncmp(token, "PUT", 3) == 0 || std::strncmp(token, "put", 3) == 0) {
    // Do PUT command
    token = std::strtok(NULL, delim_);

    if (token == NULL) {
      throw std::runtime_error("PUT Error: Expected key, got NULL");
    }

    char *value = std::strtok(NULL, "");

    if (value == NULL) {
      throw std::runtime_error("PUT Error: Expected value, got NULL");
    }

    return Request(token, value);
  } else {
    throw std::runtime_error(utils::toString("Unknown command: ", token));
  }
  */



/*
Response Response::deserialize(const std::string& data) {
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

/*
Request Request::deserialize(const std::string& data) {
  boost::char_separator<char> sep{" "};
  boost::tokenizer<boost::char_separator<char>> tok{data, sep};
  auto it = tok.begin();

  if (*it == "GET" || *it == "get") {
    // Do GET command
    if ((*it).size() == data.size()) {
      throw std::runtime_error("GET Error: Expected key, got NULL");
    }

    return Request(data.substr((*it).size()));
  } else if (*it == "PUT" || *it == "put") {
    // Do PUT command
    if ((*it).size() == data.size()) {
      throw std::runtime_error("PUT Error: Expected key, got NULL");
    }

    it++;
    std::string key = *it;










    char *value = std::strtok(NULL, "");

    if (value == NULL) {
      throw std::runtime_error("PUT Error: Expected value, got NULL");
    }

    return Request(token, value);


  } else {
    throw std::runtime_error(utils::toString("Unknown command: ", *it));
  }
}
  */
