#include "format.h"

#include <sstream>
#include <string>

using std::string;

/**
 * @brief Format value to double cipher. It is useful for date format.
 *
 * @param value  Value to be formatted.
 * @return std::string Value of rwo ciphers
 */
static std::string formatValue(long value) {
  std::string name;
  if (value < 10) {
    name = "0" + std::to_string(value);

  } else {
    name = std::to_string(value);
  }
  return name;
}
/**
 * @brief ElapsedTime is an helper function that transform
 * uptime seconds in the format HH:MM:SS
 *
 * @param seconds  time in seconds.
 * @return string formatted time HH::MM::SS
 */
string Format::ElapsedTime(long int value) {
  if (value < 0) {
    throw std::invalid_argument("time cannot be negative");
  }
  auto hour = value / 3600;
  auto rest = value % 3600;
  auto minutes = rest / 60;
  auto seconds = rest % 60;
  // it's very verbose but we don't have std::format C++20 here.
  // i could use C's printf but i preferred to be stick to streams.
  std::ostringstream os;
  os << formatValue(hour);
  os << ":";
  os << formatValue(minutes);
  os << ":";
  os << formatValue(seconds);
  return os.str();
}