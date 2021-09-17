#ifndef CPU_SAMPLER_H
#define CPU_SAMPLER_H
#include <functional>
#include <string>

namespace LinuxParser {
/**
 * @brief This class cpuSampler has the single responsiblity to
 * sample the cpu over ksamples, each every 100ms and return the median.
 * This is pretty useful for having a better cpu utilization.
 *
 */
class CPUSampler final {
public:
  /**
   * @brief Construct a new cpu Sampler object
   *
   * @param samples number of samples for cpu usage.
   * @param func callback for the results.
   */
  CPUSampler(int samples, const std::function<void(float)> &func);
  /**
   * @brief start the sampling when it is finished will call back the function.
   *
   */
  void Sample();
  /**
   * @brief Sampling time
   *
   */
  static constexpr int SAMPLING_TIME_MS{100};

private:
  // load data
  float LoadData() const;
  // convert the /proc/stat counter in vector of values
  std::vector<int> Split(const std::string &input) const;
  // number of samples
  int samples_;
  // callback
  std::function<void(float)> update_;
};
} // namespace LinuxParser
#endif
