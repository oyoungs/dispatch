
#include <oyoung/range.hpp>
#include <gtest/gtest.h>


TEST(oyoung_range, range_9) {
    auto n {0};
    for(auto i: oyoung::range(9)) {
        EXPECT_LT(i, 9);
        EXPECT_GE(i, 0);
        EXPECT_EQ(n++, i);
    }
}

TEST(oyoung_range, range_1_to_10) {
    auto n {1};
    for(auto i: oyoung::range(1, 10)) {
        EXPECT_GE(i, 1);
        EXPECT_LT(i, 10);
        EXPECT_EQ(n++, i);
    }
}

TEST(oyoung_range, range_except_for_step_0) {
    EXPECT_NO_THROW(oyoung::range(0, 10));
    EXPECT_ANY_THROW(oyoung::range(0, 10, 0));
}

TEST(oyoung_range, range_20_downto_10) {
    auto n {20};
    for(auto i: oyoung::range(20, 10)) {
        EXPECT_LE(i, 20);
        EXPECT_GT(i, 10);
        EXPECT_EQ(n--, i);
    }
}

TEST(oyoung_range, range_1_to_10_step_3) {
    auto n {1};
    for(auto i: oyoung::range(1, 10, 3)) {
        EXPECT_GE(i, 1);
        EXPECT_LT(i, 10);
        EXPECT_EQ(n, i);
        n += 3;
    }
}

TEST(oyoung_range, range_20_downto_10_step_2) {
    auto n {20};
    for(auto i: oyoung::range(20, 10, 2)) {
        EXPECT_EQ(n, i);
        EXPECT_GT(i, 10);
        EXPECT_LE(i, 20);
        n -= 2;
    }
}

TEST(oyoung_range, repeat_5_with_index) {
    auto n {0};
    repeat(5) [&](int i) {
        EXPECT_GE(i, 0);
        EXPECT_LT(i, 5);
        EXPECT_EQ(n++, i);
    };
}

TEST(oyoung_range, repeat_3_without_index) {
    auto n {0};
    repeat(3) [&] {
        EXPECT_GE(n, 0);
        EXPECT_LT(n++, 3);
    };
}

TEST(oyoung_range, repeater_6_with_index) {
    auto n {0};
    oyoung::Repeater(6) << [&](int i) {
        EXPECT_GE(i, 0);
        EXPECT_LT(i, 6);
        EXPECT_EQ(n++, i);
    };
}

TEST(oyoung_range, contains) {
    EXPECT_TRUE(oyoung::range(5).contains(3)) << " for range(5) contains 3";
    EXPECT_FALSE(oyoung::range(3).contains(5)) << " for range(3) not contains 5";
    EXPECT_TRUE(oyoung::range(5, 9).contains(5)) << " for range(5, 9) contains 5";
    EXPECT_FALSE(oyoung::range(5, 9).contains(9)) << " for range(5, 9) contains 9";
    EXPECT_TRUE(oyoung::range(1, 100, 7).contains(8)) << " for range(5, 9) contains 5";
    EXPECT_FALSE(oyoung::range(1, 100, 7).contains(9)) << " for range(5, 9) contains 9";
}

