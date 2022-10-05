#pragma once

#include <cstdio>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <boost/optional.hpp>


namespace utils {

template<typename T>
std::string _toString(std::stringstream& stream, const T& v) {
  stream << v;
  return stream.str();
}

template<typename T, typename... Args>
std::string _toString(std::stringstream& stream, const T& v, Args... args) {
  stream << v;
  return _toString(stream, args...);
}

/**
 * Convert many objects to a string.
 *
 * @param v: Object of type T that can be passed to std::stringstream.
 * @param args: Remaining objects to be converted.
 * @return: String representation of passed in objects.
 */
template<typename T, typename... Args>
std::string toString(const T& v, Args... args) {
  std::stringstream stream;
  return _toString(stream, v, args...);
}

/**
 * Convert an object to a string.
 *
 * @param v: Object of type T that can be passed to std::stringstream.
 * @return: String representation of passed in object.
 */
template<typename T>
std::string toString(const T& v) {
  std::stringstream stream;
  stream << v;
  return stream.str();
}

std::string makePutCommand(const std::string& key, const std::string& value);

std::string makeGetCommand(const std::string& key);

template <typename Out>
void split(const std::string &s, char delim, Out result, uint32_t maxsplit = INT_MAX) {
    std::istringstream iss(s);
    std::string item;
    int i = 0;
    while (i < maxsplit && std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim, uint32_t maxsplit = INT_MAX);

} // namespace utils
