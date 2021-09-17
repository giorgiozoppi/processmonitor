#include "catch2/catch.hpp"
#include "system.h"
#include <fstream>

TEST_CASE("Shall be the kernel version correct", "[system]") {
    System currentSystem;
    REQUIRE("5.10.16.3-microsoft-standard-WSL2" == currentSystem.Kernel());
}
TEST_CASE("Shall be the number of running processes correct", "[system]") {
    System currentSystem;
    REQUIRE(1 == currentSystem.RunningProcesses());
}
TEST_CASE("Shall be the number of total processes correct", "[system]") {
    System currentSystem;
    std::system("ps -e | wc -l > /tmp/numproc");
    int num_proc{0};
    std::ifstream proc{"/tmp/numproc"};
    if (proc.is_open()) {
        proc >> num_proc;
    }
    // remove bash and ps
    num_proc-=4;
    REQUIRE(num_proc == currentSystem.TotalProcesses());
}
TEST_CASE("Shall be the cpu model correct", "[system]") {
    System system;
    auto firstCore = system.Cpu();
    REQUIRE("Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz" == firstCore.ModelName());
    REQUIRE(firstCore.Frequency() == 2712.00);
    REQUIRE(firstCore.CacheSize() == 3072);
}
TEST_CASE("Shall be the uptime greater than zero", "[system]") {
    System system;
    REQUIRE(0 < system.UpTime());
}
TEST_CASE("Shall be the Ubuntu version correct", "[system]") {
    System system;
    REQUIRE("Ubuntu 20.04.3 LTS" == system.OperatingSystem());
}
