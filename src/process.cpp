#include "process.h"

#include <unistd.h>

#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cpu_sampler.h"
#include "linux_parser.h"
#include "processor.h"
#include "util.h"

using std::string;
using std::to_string;
using std::vector;
using util::is_number;
using util::ltrim;
using util::rtrim;
using util::scan_pid;
using util::split;
using util::splitInTwo;

/**
 * @brief Build a process information using the directory.
 *
 * @param directory
 * @return Process
 */
Process ProcessBuilder::Build(const std::filesystem::path &directory) {
  Process p;
  std::string pid(directory.filename());
  std::filesystem::path procDir(LinuxParser::kProcDirectory);
  procDir += pid;

  if (!is_number(pid)) {
    throw std::invalid_argument("uid shall be a number");
  }

  p.pid_ = std::stoi(pid);
  p.user_ = FindUser(procDir);
  p.uptime_ = FindUptime(procDir);
  p.cpu_usage_ = FindCpuUsage(procDir);
  p.command_ = FindCommand(procDir);
  p.ram_ = FindMemoryUsage(procDir);
  return p;
}
/**
 * @brief Find the uptime looking in /proc/pid
 *
 * @param base subdirectory /proc/PID
 * @return long the value in uptime.
 */
long ProcessBuilder::FindUptime(const std::filesystem::path &base) {
  std::filesystem::path userstat{base};
  userstat += LinuxParser::kStatFilename;
  std::ifstream userdata{userstat};
  if (userdata.is_open()) {
    auto contents = util::readlines(userdata);
    auto fields = util::split(contents, ' ');
    if (fields.size() < 22) {
      return 0;
    }
    /*
     * In the spec we've the uptime(field 21) but both top and htop use the
     * utime + stime for TIME+, we've decided to be close to htop and top. In
     * we'd to use field 21, no need to multiply * 60. we've chosed to get stick
     * to the standard and have a comparison during testing.
     * I checked the code in htop. If we launch htop or top we'll have the same
     * values.
     */
    auto update = 60 * (std::stol(fields[13]) + std::stol(fields[14])) /
                  sysconf(_SC_CLK_TCK);
    return update;
  }
  return 0;
}

/**
 * @brief Find a user associated to the process
 *
 * @param base   base directory in /proc/PID
 * @return std::string a string containing the username.
 */
std::string ProcessBuilder::FindUser(const std::filesystem::path &base) {
  std::filesystem::path userstat{base};
  userstat += LinuxParser::kStatusFilename;
  std::ifstream userdata{userstat};
  if (userdata.is_open()) {
    std::string data;
    std::string uid;
    while (std::getline(userdata, data)) {
      if (data.find("Uid") != std::string::npos) {
        auto [first, second] = util::splitInTwo(data, ":");
        // i use just the second part and remove blanks
        util::replace(second, "\t", "");
        auto values = util::split(second, '\t');
        uid = values[0];
        // now we've to open /etc/passwd
        break;
      }
    }
    // so we've the uid
    std::ifstream userdb{LinuxParser::kPasswordPath};
    // we scan until we found  a line with the id.
    if (userdb.is_open()) {
      while (std::getline(userdb, data)) {
        if (data.find(uid) != std::string::npos) {
          auto fields = util::split(data, ':');
          if ((fields.size() > 2) &&
              (is_number(fields[2]) && fields[2] == uid)) {
            // ok we've done.
            return fields[0];
          }
        }
      }
    }
  }
  return "";
}
/**
 * @brief Find the cpu usage for the current process.
 *
 * @param base  path in /proc for the current process
 * @return float cpu usage for the current process
 */
float ProcessBuilder::FindCpuUsage(const std::filesystem::path &base) {
  std::filesystem::path path{base};
  std::filesystem::path procBase{LinuxParser::kProcDirectory};
  std::filesystem::path globalStatPath{procBase};
  globalStatPath += LinuxParser::kStatFilename;

  auto cpus = DetectProcessor::GetSystemProcessors();
  auto num_cpus{1};
  if (cpus == std::nullopt) {
    num_cpus = 1;
  } else {
    num_cpus = cpus.value().size();
  }
  // we get the total cpu usage
  auto totalCpuUsage = GetTotalCpuUsage(num_cpus);
  std::filesystem::path procStatPath{path};
  procStatPath += LinuxParser::kStatFilename;

  std::ifstream procStat{procStatPath};

  auto contents = util::readlines(procStat);
  if (contents.size() != 0) {
    auto usage_data = util::split(contents, ' ');
    float utime = stof(usage_data[13]);
    float stime = stof(usage_data[14]);
    auto cpu_usage = (utime + stime) / totalCpuUsage;
    return cpu_usage;
  }
  return 0.0f;
}
/**
 * @brief Find memory usage for the current process
 *
 * @param base path in /proc for the current process
 * @return std::string memory usage.
 */
std::string ProcessBuilder::FindMemoryUsage(const std::filesystem::path &base) {
  std::filesystem::path path{base};
  path += LinuxParser::kStatusFilename;
  std::ifstream stream{path};
  std::string current;

  if (stream.is_open()) {
    while (std::getline(stream, current)) {
      if (current.find("VmSize") != std::string::npos) {
        auto [first, second] = util::splitInTwo(current, ":");
        util::replace(second, "kB", "");
        util::ltrim(second);
        util::rtrim(second);
        auto ram = std::to_string(stof(second) / 1024.0f);
        return ram.substr(0, ram.find(".") + 2);
      }
    }
  }
  return "";
}

/**
 * @brief Find the current command for the current process
 *
 * @param base  path in /proc for the current process
 * @return std::string
 */
std::string ProcessBuilder::FindCommand(const std::filesystem::path &base) {
  std::filesystem::path path;
  std::string contents;
  contents.resize(2048);
  path += base;
  path += "/cmdline";
  std::ifstream current{path};
  std::getline(current, contents);
  if (contents.size() == 0) {
    // we look at /status first line and split
    std::filesystem::path status;
    status += base;
    status += "/status";
    std::ifstream current{status};
    std::getline(current, contents);
    auto v = util::split(contents, ':');
    util::ltrim(v[1]);
    util::ltrim(v[1]);
    current.close();
    return v[1];
  } else {
    auto exe = util::split(contents, '\00');
    return exe[0];
  }
  return contents;
}
/**
 * @brief Return the average totaltime
 *
 * @return unsigned long long int
 */
unsigned long long int ProcessBuilder::GetTotalCpuUsage(unsigned int num_cpu) {
  std::filesystem::path path{LinuxParser::kProcDirectory};
  path += LinuxParser::kStatFilename;
  std::ifstream data{path};
  std::string row;
  if (data.is_open()) {
    std::getline(data, row);
    // this part is inspired by htop code in LinuxProcessList.c
    // there is a part where for CPU computtes the period
    // we're doing something similar here.

    auto fields = util::split(row, ' ');
    auto usertime = stoul(fields[2]);
    auto nicetime = stoul(fields[3]);
    auto systemtime = stoul(fields[4]);
    auto idletime = stoul(fields[5]);
    auto ioWait = stoul(fields[6]);
    auto irq = stoul(fields[7]);
    auto softIrq = stoul(fields[8]);
    auto steal = stoul(fields[9]);
    auto guest = stoul(fields[10]);
    auto guestnice = stoul(fields[11]);
    // this is based on htop calculation.
    usertime = usertime - guest;
    nicetime = nicetime - guestnice;
    unsigned long long int idlealltime = idletime + ioWait;
    unsigned long long int systemalltime = systemtime + irq + softIrq;
    unsigned long long int virtalltime = guest + guestnice;
    // total global time spent on average in the cpu + io.
    unsigned long long int totaltime =
        usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;
    return totaltime / num_cpu;
  }
  return 0;
}
int Process::Pid() const noexcept { return pid_; }

float Process::CpuUtilization() const noexcept { return cpu_usage_; }
string Process::Command() const noexcept { return command_; }

string Process::Ram() const noexcept { return ram_; }

string Process::User() const noexcept { return user_; }

long int Process::UpTime() const noexcept { return uptime_; }

bool Process::operator<(Process const &a) const { return this->pid_ < a.pid_; }