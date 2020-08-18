//
// Created by oyoung on 19-7-8.
//

#include <oyoung/defer.hpp>
#include <gtest/gtest.h>

TEST(oyoung_defer, defer_for_func) {

    USE_DEFER;


    auto n {0};

    defer [&] {
        EXPECT_EQ(100, n);
    };

    n = 100;
}

TEST(oyoung_defer, defer_for_scope) {

    auto n {0};

    {
        USE_DEFER;


        defer [&] {
            n = 0;
        };

        n = 2;
    }

    EXPECT_EQ(0, n);
}