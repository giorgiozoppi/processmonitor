#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <fstream>
#include <optional>
#include <string>
#include <vector>

// forward declaration
class Processor;

/**
 * @brief DetectProcessor is a factory class that detects the current cores
 * in the system.
 */
class DetectProcessor final {
 public:
  /**
   * @brief Get the System Processors (Cores) Descriptors if any parse
   * error occurs the list can be std::nullopt.
   * @return std::optional<std::vector<Processor>>
   */
  static std::optional<std::vector<Processor>> GetSystemProcessors();

 private:
  static std::optional<Processor> CreateProcessor(std::ifstream& stream);
  static std::tuple<std::string, std::string> ParseLine(
      const std::string& line);
};
/**
 * @brief Processor is a class that models a single core.
 *
 */
class Processor final {
  // This allows us to avoid to create a constructor for processor.
  // we use DetectProcessor like an object factory
  friend class DetectProcessor;

 public:
  /**
   * @brief CPU_SAMPLES is the number of sample to use for Utilization
   *
   */
  static constexpr unsigned int CPU_SAMPLES = 10;

  /**
   * @brief CacheSize returns the cache size of the processor.
   *
   * @return int size of the cache in KB.
   */
  int CacheSize() const;

  /**
   * @brief Frequency returns the frequency of the processor.
   *
   * @return float  clock frequency in Mhz.
   */
  float Frequency() const;
  /**
   * @brief ModelName returns the model name of the processor
   *
   * @return std::string
   */
  std::string ModelName() const;
  /**
   * @brief Utilization does sampling and return current utilization
   *
   * @return float the median of sampled utilization values
   */
  float Utilization();

 private:
  std::string modelName_{""};
  float frequency_{0.0f};
  float cacheSize_{0.0f};
};

#endif