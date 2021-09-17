#include "catch2/catch.hpp"
#include "format.h"

TEST_CASE("Shall format correctly second", "[format]") {
    long value = 3600 + (10 * 60) + 55;
    std::string expected_value{"01:10:55"};
    auto actual_value = Format::ElapsedTime(value);
    REQUIRE(expected_value == actual_value);
}
TEST_CASE("Shall correctly throw with negative values", "[format]") {
    long value{-1};
    bool throwed{false};
    try {
        Format::ElapsedTime(value);
    } catch(...) {
        throwed = true;
    }
    REQUIRE(true == throwed);
}
