//
// Created by oyoung on 18-12-10.
//

#include <oyoung/format.hpp>
#include <gtest/gtest.h>


TEST(oyoung_format, replace) {
    EXPECT_EQ("Hello, C++, C++, C++!!!", oyoung::format("Hello, world, world, world!!!").replace("world", "C++").to_string());
}

TEST(oyoung_format, arg_pod) {
    EXPECT_EQ("My name is oyoung, my age is 26, my gender is male, my height is 175.0m", oyoung::format("My name is %1, my age is %2, my gender is %3, my height is %4m")
            .arg("oyoung")
            .arg(26)
            .arg("male")
            .arg(175.0, 1)
            .to_string());
}

TEST(oyoung_format, strip) {
    EXPECT_EQ("123456789", oyoung::strip("\n\t\r123456789\t\r\n"));
    EXPECT_EQ("123456789", oyoung::lstrip("0000123456789", "0"));
    EXPECT_EQ("123456789", oyoung::rstrip("123456789****", "*"));
    EXPECT_EQ("", oyoung::strip("\r\n\t"));
}

TEST(oyoung_format, time_format) {
    EXPECT_EQ("Thu Jan  1 08:00:00 1970", oyoung::formatter::time_format(std::chrono::system_clock::time_point{}, "%Ec"));
    EXPECT_EQ("1970-01-01 08:00:00", oyoung::format("%1").arg(std::chrono::system_clock::time_point{}, "%Y-%m-%d %H:%M:%S").to_string());
}
