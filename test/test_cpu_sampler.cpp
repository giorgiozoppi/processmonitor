#include "catch2/catch.hpp"
#include "cpu_sampler.h"
TEST_CASE("Should compute value", "[cpu_sampler]") {
    float data{0.0};
    LinuxParser::CPUSampler sampler{6, [&data](float current){
        data = current;
    }};
    sampler.Sample();
    REQUIRE(data > 0.0);
    REQUIRE(data < 100.0);
}
