#ifndef PROCESS_H
#define PROCESS_H

#include <filesystem>
#include <string>
// forward declaration
class Process;

/**
 * @brief ProcessBuolder is a builder class for the Process,
 * it scans the /proc/PID and fetch all the values.
 * We separate the logic of creation from the Process such that
 * Process class is just a readonly class.
 */
class ProcessBuilder final {
 public:
  /**
   * @brief Build a process passing its path.
   *
   * @param process_dir
   * @return Process
   */
  static Process Build(const std::filesystem::path& process_dir);

 private:
  /**
   * @brief Find the process user information using the /proc fs.
   *
   * @param base path in proc of the current process
   * @return std::string user name of the current process user.
   */
  static std::string FindUser(const std::filesystem::path& base);
  /**
   * @brief Find uptime for the current process
   *
   * @param base  path in proc fo the current base
   * @return long time in seconds of the uptime.
   */
  static long FindUptime(const std::filesystem::path& base);
  /**
   * @brief Find the cpu usage for the current process.
   *
   * @param base  path in /proc for the current process
   * @return float cpu usage for the current process
   */
  static float FindCpuUsage(const std::filesystem::path& base);
  /**
   * @brief Find memory usage for the current process
   *
   * @param base path in /proc for the current process
   * @return std::string memory usage.
   */
  static std::string FindMemoryUsage(const std::filesystem::path& base);

  /**
   * @brief Find the current command for the current process
   *
   * @param base  path in /proc for the current process
   * @return std::string
   */
  static std::string FindCommand(const std::filesystem::path& base);
  /**
   * @brief Get the average cpu total time.
   * We'll use this for computing the process time.
   *
   * @param num_cpu Number of the cpu in the system
   * @return unsigned long long int
   */

  static unsigned long long int GetTotalCpuUsage(unsigned int num_cpu);
};

/**
 * @brief Class for modeling a process in the monitor
 * It represent a process and what we need.
 */
class Process final {
 public:
  /**
   * @brief Pid   Process ID
   *
   * @return int  the number of process id
   */

  int Pid() const noexcept;
  /**
   * @brief User  User in the system for the process.
   *
   * @return std::string username associated to the process.
   */
  std::string User() const noexcept;
  /**
   * @brief Command command line argument for the process.
   *
   * @return std::string A command associated with the process.
   */
  std::string Command() const noexcept;
  /**
   * @brief CpuUtilization current CPU usage for this process
   *
   * @return float  the percentuage of CPU usage for this process.
   */
  float CpuUtilization() const noexcept;
  /**
   * @brief Ram used for this process.
   *
   * @return std::string
   */
  std::string Ram() const noexcept;
  /**
   * @brief Uptime for this process.
   *
   * @return long int uptime
   */
  long int UpTime() const noexcept;
  /**
   * @brief A comparator opertator for sorting the processes.
   *
   * @param a     Process to compare
   * @return true If is less then a
   * @return false otherwise.
   */
  bool operator<(Process const& a) const;

 private:
  int pid_;
  std::string user_;
  std::string command_;
  std::string ram_;
  long int uptime_;
  float cpu_usage_;
  // this is because i want encapsulate the creation.
  // I dont want to give to the user to do a new Process();
  // the alternative can be creat constructor with k params
  // or setters but it's a bit more code.
  friend ProcessBuilder;
};

#endif