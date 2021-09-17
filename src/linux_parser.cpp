#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cmath>
#include <string>
#include <vector>

#include "util.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using util::ltrim;
using util::readlines;
using util::rtrim;
using util::scan_pid;
using util::splitInTwo;

/**
 * @brief Retrieve the operating system, i.e the Linux flavor
 *
 * @return string The operating system flavor.
 */
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}
/**
 * @brief Return the current kernel
 *
 * @return string  Kernel version.
 */
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/**
 * @brief List current processes PID
 *
 * @return vector<int> a list of current pids.
 */
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  scan_pid(kProcDirectory,
           [&pids](std::string pidname) { pids.push_back(stoi(pidname)); });
  return pids;
}

/**
 * @brief Read the global memory utilization. The formula is simple total -
 * available.
 *
 * @return float the global memory utilization in megabytes
 */
float LinuxParser::MemoryUtilization() {
  std::filesystem::path path{LinuxParser::kProcDirectory};
  path += LinuxParser::kMeminfoFilename;
  std::ifstream data{path};
  if (data.is_open()) {
    std::string row;
    short row_number = 0;
    float mem_total{0};
    float mem_avail{0};

    while (std::getline(data, row)) {
      util::replace(row, "kB", "");
      if (row_number == 0) {
        auto [key, value] = splitInTwo(row, ":");
        ltrim(value);
        rtrim(value);
        mem_total = stof(value);

      } else if (row_number == 1) {
        auto [key, value] = splitInTwo(row, ":");
        ltrim(value);
        rtrim(value);
        mem_avail = stof(value);
        break;
      }
      row_number++;
    }
    return ((mem_total - mem_avail) / mem_total);
  }
  return 0.0;
}

/**
 * @brief Read the uptime. The time since the last reboot
 *
 * @return long  the rime in seconds since last reboot.
 */
long LinuxParser::UpTime() {
  std::filesystem::path path{LinuxParser::kProcDirectory};
  path += LinuxParser::kUptimeFilename;
  std::ifstream data{path};
  if (data.is_open()) {
    std::string current{""};
    std::getline(data, current);
    auto [uptime, idle] = splitInTwo(current, " ");
    // get promoted to long.
    return std::round(stof(uptime));
  }
  return 0;
}

/**
 * @brief Read and return the number of processes in the system.
 *
 * @return int number of current processes in the system.
 *         0 in case of error.
 */
int LinuxParser::TotalProcesses() {
  int proc_number{0};
  // does an equivalent of python, os.listdir and it applies
  // the lambda to each element
  // it's like std::transform but listing a filesystem
  // scan_pid is used twice: here and when we build the processes.

  scan_pid(LinuxParser::kProcDirectory,
           [&proc_number](const std::string &) { proc_number++; });
  return proc_number;
}
/**
 * @brief Return the running processes.
 *
 * @return int the number of processing executing in a CPU. In case of error
 * reports 0.
 */
int LinuxParser::RunningProcesses() {
  std::filesystem::path path{LinuxParser::kProcDirectory};
  path += LinuxParser::kStatFilename;
  std::ifstream data{path};
  if (data.is_open()) {
    std::string row;
    while (std::getline(data, row)) {
      if (row.find("procs_running") != std::string::npos) {
        // we don't use key but just value in this structured binding
        auto [key, value] = util::splitInTwo(row, " ");
        return util::to_integral(value);
      }
    }
  }
  return 0;
}
