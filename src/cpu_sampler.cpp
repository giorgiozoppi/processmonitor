#include "cpu_sampler.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <numeric>
#include <thread>
#include <vector>

#include "linux_parser.h"

namespace LinuxParser {

/**
 * @brief Construct a new CPUSampler::CPUSampler object
 *
 * @param samples number of samples
 * @param func    tcallback to call when the samping and median computation is
 * done
 */
CPUSampler::CPUSampler(int samples, const std::function<void(float)>& func)
    : samples_(samples), update_(func) {}

/**
 * @brief Sample the cpu load and compute the median
 *
 */
void CPUSampler::Sample() {
  std::vector<float> data;
  for (auto times = 0; times < this->samples_; ++times) {
    auto item = LoadData();
    if (item > 0) {
      data.emplace_back(item);
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(CPUSampler::SAMPLING_TIME_MS));
  }
  std::sort(data.begin(), data.end());
  auto sample_size = data.size();
  auto pos =
      (sample_size % 2 == 0) ? sample_size / 2 : std::round(sample_size / 2);
  auto median = data[pos];
  update_(median);
}
/**
 * @brief split the string to a vector (parse the string)
 * @param input string of counters with spaces (i.e 912 129 12 12)
 * @return std::vector<int> a vector of integer
 */
std::vector<int> CPUSampler::Split(const std::string& input) const {
  std::istringstream tokens(input);
  std::vector<int> data;
  while (tokens) {
    int v{0};

    tokens >> v;
    if (tokens) {
      data.emplace_back(v);
    }
  }
  return data;
}
/**
 * @brief Load data from /proc/stat about CPU usage
 *  we return 0 when we cannot read the data.
 *
 * @return float
 */
float CPUSampler::LoadData() const {
  // build the correct file path before opening the file
  std::filesystem::path dataPath(LinuxParser::kProcDirectory);
  dataPath += "stat";
  std::ifstream datafile{dataPath};
  if (datafile.is_open()) {
    std::string cpuValue;
    std::getline(datafile, cpuValue);
    cpuValue = cpuValue.substr(5);
    auto data = Split(cpuValue);
    // precondition shall be at least 7.
    if (data.size() < 7) {
      // this sampling has been not correct;
      return 0.0;
    }
    /*  Those are the semantic of the values in the data array:
        1st item : user = normal processes executing in user mode
        2nd column : nice = niced processes executing in user mode
        3rd column : system = processes executing in kernel mode
        4th column : idle = twiddling thumbs
        5th column : iowait = waiting for I/O to complete
        6th column : irq = servicing interrupts
        7th column : softirq = servicing softirqs
    */
    auto idle_percentuage =
        (data[3] * 100.0) /
        std::reduce(std::execution::par, data.begin(), data.begin() + 7);
    return 100.0 - idle_percentuage;
  }
  return 0.0;
}
}  // namespace LinuxParser
