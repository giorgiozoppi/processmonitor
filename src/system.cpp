#include "system.h"

#include <unistd.h>

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "util.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
using util::ltrim;
using util::rtrim;
using util::scan_pid;
using util::splitInTwo;
using util::to_float;
using util::to_integral;
/**
 * @brief Construct a new System
 * Here we call some functions for loading values.
 * We load first immutable values.
 */
System::System() {
  // we initialize the values.
  DetectKernelVersion();
  auto result = DetectProcessor::GetSystemProcessors();
  if (result != std::nullopt) {
    auto coresArray = result.value();
    if (coresArray.size() > 0) {
      cpu_ = coresArray[0];
    }
  }
  DetectOperatingSystem();
}
/**
 * @brief Return the CPU
 *
 * @return Processor& A processor descriptor.
 */

Processor& System::Cpu() { return cpu_; }

/*
 */
// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  std::filesystem::path base{LinuxParser::kProcDirectory};
  processes_.clear();
  auto processes = LinuxParser::Pids();
  for (const auto& process : processes) {
    auto current_proc = base;
    current_proc += std::to_string(process);
    auto process_data = ProcessBuilder::Build(current_proc);
    processes_.emplace_back(process_data);
  }
  return processes_;
}

/**
 * @brief Kernel returns the version of the kernel of the current Linux OS.
 *
 * @return std::string kernel version
 */

std::string System::Kernel() { return kernel_version_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

//  Return the operating system name
std::string System::OperatingSystem() { return operating_system_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() {
  running_processes_ = LoadRunningProcessing();
  return running_processes_;
}

/**
 * @brief Return the number of total processes scanning /proc directory
 * The proc directory on Linux contains a directory for each process with the
 * pid name. The pid me is just a number
 * @return int the number of processes present in the system.
 */
int System::TotalProcesses() {
  total_processes_ = LinuxParser::TotalProcesses();
  return total_processes_;
}

/**
 * @brief Return the number of seconds since the system started running
 *
 * @return long int number of second from the last boot.
 */
long int System::UpTime() {
  uptime_ = LinuxParser::UpTime();
  return uptime_;
}

int System::LoadRunningProcessing() { return LinuxParser::RunningProcesses(); }
void System::DetectKernelVersion() { kernel_version_ = LinuxParser::Kernel(); }
void System::DetectOperatingSystem() {
  operating_system_ = LinuxParser::OperatingSystem();
}