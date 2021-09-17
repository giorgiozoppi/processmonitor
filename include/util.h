#ifndef ND_UTIL_H
#define ND_UTIL_H
#include <algorithm>
#include <filesystem>
#include <functional>
#include <sstream>
#include <string>
#include <tuple>

/**
 * @brief This namespace has a useful string parsing functions
 * to make it easy the parsing and splitting of text files in /proc.
 */
namespace util {
// string_view since C++17 is a literal type
static constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
static constexpr std::string_view NUMBERS = "0123456789";

/**
 * @brief  Replace substring with a new string in a original string.
 *         Replace follows the pattern of other languages where you provide a
 * substring to replace without looking for the start position where you have to
 * replace.
 *
 * @param str    Original string passed by reference
 * @param repl   String to replace from the original string
 * @param torepl New string to be inserted into the original string
 * @return std::string a copy of the original string replaced. Note that it's
 * cheap due to NVO.
 */
inline std::string replace(std::string& str, const std::string& repl,
                           const std::string& torepl) {
  if (str.find(repl) == std::string::npos) {
    return str;
  }
  return str.replace(str.find(repl), repl.length(), torepl);
}
/**
 * @brief Read all the file content in a string. It's like Python's readlines.
 *
 * @param stream        stream to be used.
 * @return std::string  resulting string.
 */
inline std::string readlines(std::ifstream& stream) {
  std::string current;
  std::ostringstream out;

  if (stream.is_open()) {
    while (std::getline(stream, current)) {
      out << current;
    }
  }
  return out.str();
}
/**
 * @brief Trim at end of the string of all whitespaces creating a new string.
 * @param value string to be trimmed.
 * @return std::string
 */
inline std::string rtrim(std::string& value) {
  size_t end = value.find_last_not_of(WHITESPACE);
  if (end != std::string::npos) {
    value = value.substr(0, end + 1);
  }
  return value;
}
/**
 * @brief Trim at beginning of the string of all whitespaces creating a new
 * string.
 * @param value string to be trimmed.
 * @return std::string
 */
inline std::string ltrim(std::string& value) {
  size_t start = value.find_first_not_of(WHITESPACE);
  if (start != std::string::npos) {
    value = value.substr(start);
  }
  return value;
}
/**
 * @brief Split in two parts a string in a tuple<string,string>.
 * @param str string to be splitted
 * @param chars list of characters to be used.
 * @return std::string
 */
inline std::tuple<std::string, std::string> splitInTwo(
    const std::string& str, const std::string_view chars) {
  auto mid = str.find_first_of(chars);
  auto first = str.substr(0, mid);
  auto second = str.substr(mid + 1);
  ltrim(first);
  rtrim(second);
  return std::make_tuple(first, second);
}
/**
 * @brief Split a string using a separator character
 *
 * @param data String to be splitted
 * @param sep Separator character
 * @return std::vector<std::string> Array of substrings divided by separator.
 */
inline std::vector<std::string> split(const std::string& data, char sep) {
  std::vector<std::string> splitted_data;
  auto prev = 0;
  auto current = data.find_first_of(sep);
  while (current != std::string::npos) {
    auto first = data.substr(prev, current - prev);
    ltrim(first);
    splitted_data.emplace_back(first);
    prev = current + 1;
    current = data.find_first_of(sep, prev);
  }
  if (prev > 0) {
    auto first = data.substr(prev);
    ltrim(first);
    splitted_data.emplace_back(first);
  }
  return splitted_data;
}

/**
 * @brief Check if a string is an integer number and it could be converted.
 *
 * @param data string to be checked.
 * @return true  if it's a integral number
 * @return false if it's not an integral number
 */
inline bool is_number(const std::string& data) {
  return ((!data.empty()) &&
          (data.find_first_not_of(NUMBERS) == std::string::npos));
}
inline int to_integral(const std::string& row) {
  auto value(row);
  ltrim(value);
  rtrim(value);
  return stoi(value);
}
inline float to_float(const std::string& row) {
  auto value(row);
  ltrim(value);
  rtrim(value);
  return stof(value);
}
/**
 * @brief Scan a directory applying a callback function to each element
 * if this item has a value convertible to an integer number. This is used
 * to scan for process id in /proc. It could be used to count the number of
 * processes or to analyze each subdirectory.
 *
 * @param path path to be scanned.
 * @param op   callback to be called foreach item.
 */
inline void scan_pid(const std::string& path,
                     const std::function<void(std::string)>& op) {
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    auto pathname = entry.path();
    if (entry.is_directory() && util::is_number(pathname.filename())) {
      op(pathname.filename());
    }
  }
}
}  // namespace util
#endif
