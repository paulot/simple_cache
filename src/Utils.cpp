#include "Utils.hpp"

namespace utils {

std::string makePutCommand(const std::string& key, const std::string& value) {
  return toString("PUT ", key, " ", value);
}

std::string makeGetCommand(const std::string& key) {
  return toString("GET ", key);
}

std::vector<std::string> split(const std::string &s, char delim, uint32_t maxsplit) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems), maxsplit);
    return elems;
}

} // namespace utils
