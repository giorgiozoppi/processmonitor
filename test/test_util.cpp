#include <fstream>

#include "catch2/catch.hpp"
#include "util.h"

TEST_CASE("Should convert float with success", "[util]") {
  auto value = util::to_float("12.3413");
  REQUIRE(12.3413f == value);
}
TEST_CASE("Shoule replace a substring", "[util") {
  std::string source{"Barcelona"};
  std::string replace{"celona"};
  util::replace(source, replace, "na");
  REQUIRE("Barna" == source);
}
TEST_CASE("Should convert int with success", "[util]") {
  auto value = util::to_integral("12");
  REQUIRE(12 == value);
}
TEST_CASE("Should detect all pids correctly", "[util]") {
  auto ret = std::system("ps ax > /tmp/nproc");
  REQUIRE(ret >=0);
  std::ifstream current{"/tmp/nproc"};
  std::string row;
  std::vector<int> expected_pids;
  std::vector<int> actual_pids;
  while (std::getline(current, row)) {
    // we skip the ps ax
    if (row.find("ps ax") == std::string::npos) {
      util::ltrim(row);
      util::rtrim(row);
      auto fields = util::split(row, ' ');
      util::rtrim(fields[0]);
      if (util::is_number(fields[0])) {
        expected_pids.push_back(util::to_integral(fields[0]));
      }
    }
  }
  util::scan_pid("/proc", [&](const std::string& currentPid) {
    if (util::is_number(currentPid)) {
      actual_pids.emplace_back(util::to_integral(currentPid));
    }
  });

  std::sort(begin(expected_pids), end(expected_pids));
  std::sort(begin(actual_pids), end(actual_pids));
  REQUIRE(expected_pids.size() == actual_pids.size());
  REQUIRE(
      std::equal(begin(expected_pids), end(expected_pids), begin(actual_pids)));
}

TEST_CASE("Should split a string correctly", "[util]") {
  std::string value{"the night is long"};
  auto v = util::split(value, ' ');
  REQUIRE(4 == v.size());
  REQUIRE("the" == v[0]);
  REQUIRE("night" == v[1]);
  REQUIRE("is" == v[2]);
  REQUIRE("long" == v[3]);
}
