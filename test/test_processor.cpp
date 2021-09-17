#include <filesystem>
#include <fstream>

#include "catch2/catch.hpp"
#include "processor.h"
#include "util.h"

TEST_CASE("Should detect the correct number of cpus", "[processor]") {
  auto cpus = DetectProcessor::GetSystemProcessors();
  std::filesystem::path cpuFile("/tmp/cpus");
  REQUIRE(cpus != std::nullopt);
  std::system("cat /proc/cpuinfo | grep processor | wc -l > /tmp/cpus");
  std::ifstream currentCpuFile{cpuFile};
  REQUIRE(true == currentCpuFile.is_open());
  std::string str;
  std::getline(currentCpuFile, str);
  auto expectedCpus = stol(str);
  REQUIRE(true == std::filesystem::remove(cpuFile));
  REQUIRE(expectedCpus == cpus.value().size());
}
TEST_CASE("Should detect cpu models", "[processor]") {
  auto cpus = DetectProcessor::GetSystemProcessors();
  std::filesystem::path cpuFile("/tmp/cpumodels");
  REQUIRE(cpus != std::nullopt);
  std::system(
      "cat /proc/cpuinfo | grep model.*name  | head -n 1 | awk -F : '{ print $2 }' | sed s/[[:space:]]/#/g  > /tmp/cpumodels");
  std::ifstream currentFile{cpuFile};
  std::string cpuModel;
  std::getline(currentFile, cpuModel);
  for (auto& cpu : cpus.value()) {
     auto model =  "#"+cpu.ModelName();  
     std::replace(model.begin(), model.end(), ' ', '#');
     REQUIRE(model == cpuModel);
  }
  REQUIRE(true == std::filesystem::remove(cpuFile));
}
