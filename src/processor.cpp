#include "processor.h"

#include <atomic>
#include <cmath>
#include <filesystem>

#include "cpu_sampler.h"
#include "linux_parser.h"
#include "util.h"
using util::ltrim;
using util::rtrim;

std::optional<std::vector<Processor>> DetectProcessor::GetSystemProcessors() {
  // the logic here is quite simple
  // as soon as we see a row with processor:
  // we've detected information about a new processor.
  std::vector<Processor> current_processors;
  std::filesystem::path path(LinuxParser::kProcDirectory);
  path += LinuxParser::kCpuinfoFilename;
  std::ifstream dataFile{path};
  if (!dataFile.is_open()) {
    return std::nullopt;
  }
  std::string row{""};
  while (std::getline(dataFile, row)) {
    if (row.find("processor\t:") != std::string::npos) {
      auto processor = CreateProcessor(dataFile);
      if (processor != std::nullopt) {
        // we just transfer ownership and later
        // we don't use the processor object.
        // i am not sure if vector does internally by default
        // it could be redundant.
        current_processors.emplace_back(std::move(processor.value()));
      }
    }
  }
  return current_processors;
}
std::tuple<std::string, std::string>
DetectProcessor::ParseLine(const std::string &line) {
  auto current{line};
  auto separator = current.find_first_of(":");
  auto name = line.substr(0, separator);
  auto value = line.substr(separator + 1);
  rtrim(name);
  ltrim(value);
  return std::make_tuple(name, value);
}
std::optional<Processor>
DetectProcessor::CreateProcessor(std::ifstream &dataFile) {
  std::string current{""};
  Processor proc;
  // we use this for reading the three value to be read and just those.
  int state{0};
  std::vector<std::string> infos{"model name", "cpu MHz", "cache size"};
  while (std::getline(dataFile, current) && (state < 3)) {
    auto [name, value] = DetectProcessor::ParseLine(current);
    if (current.find(infos[0]) != std::string::npos) {
      proc.modelName_ = value;
      state++;
    }
    if (current.find(infos[1]) != std::string::npos) {
      proc.frequency_ = std::round(std::stof(value));
      state++;
    }
    if (current.find(infos[2]) != std::string::npos) {
      auto pos = value.find("KB");
      value.replace(pos, value.length(), "");
      rtrim(value);
      proc.cacheSize_ = std::stoi(value);
      state++;
    }
  }
  return proc;
}
int Processor::CacheSize() const { return cacheSize_; }
float Processor::Frequency() const { return frequency_; }
std::string Processor::ModelName() const { return modelName_; }
float Processor::Utilization() {
  float cpuUsage{0.0};
  LinuxParser::CPUSampler sampler(CPU_SAMPLES, [&cpuUsage](float currentValue) {
    cpuUsage = currentValue;
  });
  sampler.Sample();
  cpuUsage /= 100;
  return cpuUsage;
}