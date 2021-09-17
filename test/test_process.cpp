#include <fstream>

#include "catch2/catch.hpp"
#include "process.h"
#include "util.h"

TEST_CASE("Should find current user", "[process]") {
  std::filesystem::path initprocess{"/proc/1"};
  auto process = ProcessBuilder::Build(initprocess);
  REQUIRE("root" == process.User());
}
TEST_CASE("Should find current ram", "[process]") {
  std::filesystem::path initprocess{"/proc/1"};
  auto process = ProcessBuilder::Build(initprocess);
  REQUIRE(process.Ram().size() > 0);
}

TEST_CASE("Should not found user", "[process]") {
  std::filesystem::path initprocess{"/proc/nightconfuse"};
  bool except{false};
  try {
    auto process = ProcessBuilder::Build(initprocess);
  } catch (...) {
    except = true;
  }
  REQUIRE(true == except);
}
TEST_CASE("Should create the same number of processes", "[process]") {
  std::vector<int> actual_pids;
  util::scan_pid("/proc", [&](const std::string& currentPid) {
    if (util::is_number(currentPid)) {
      actual_pids.emplace_back(util::to_integral(currentPid));
    }
  });

  std::filesystem::path path("/proc/");
  std::vector<Process> list_processes;
  for (const auto& pid : actual_pids) {
    std::filesystem::path data{path};
    data.append(std::to_string(pid));
    list_processes.emplace_back(ProcessBuilder::Build(data));
  }
  REQUIRE(list_processes.size() == actual_pids.size());
}
