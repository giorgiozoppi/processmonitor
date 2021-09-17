#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  /**
   * @brief Construct a new System
   * Load everything that it is immutable just once. Immutable things are:
   *  - CPU
   *  - Operating System
   *  - Kernel Version.
   * Other things are loaded whenever the method is called.
   */
  System();
  /**
   * @brief  Cpu return current processor descriptor (first core in the system)
   *
   * @return Processor&
   */
  Processor& Cpu();
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();          // TODO: See src/system.cpp
  /**
   * @brief Uptime returns current system uptime
   *
   * @return float  the current uptime in seconds
   */
  long int UpTime();
  /**
   * @brief TotalProcesses returns the number of processes in the system.
   *
   * @return int  number of processes
   */
  int TotalProcesses();
  /**
   * @brief RunningProcesses returns the number of processes allocated to the
   * CPU.
   *
   * @return int  number of processes allocated to the CPU
   */
  int RunningProcesses();
  /**
   * @brief Kernel describe the current kernel version.
   *
   * @return std::string  current Linux kernel version.
   */
  std::string Kernel();
  /**
   * @brief Operating system give us the current Linux flavor and distribution
   *
   * @return std::string
   */
  std::string OperatingSystem();

  // TODO: Define any necessary private members
 private:
  // Load the number of processes in the CPU.
  int LoadRunningProcessing();
  // Load the current operating system
  void DetectOperatingSystem();
  // Load the current kernel version
  void DetectKernelVersion();
  // uptime in seconds

  long int uptime_{0};
  // number of the total processes in the system
  int total_processes_{0};
  // number of the processes in running state
  int running_processes_{0};
  // linux kernel version
  std::string kernel_version_;
  // operating system
  std::string operating_system_;
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif