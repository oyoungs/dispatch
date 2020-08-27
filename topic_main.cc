#include <oyoung/topic.hpp>
#include <gtest/gtest.h>

TEST(oyoung_topic, subscribe_int_publish_int) {
    oyoung::topic::subscribe<int>("value", [](int value) {
        EXPECT_EQ(100, value);
    });

    oyoung::topic::publish("value", 100);
}

TEST(oyoung_topic, subscribe_int_publish_double) {
    oyoung::topic::subscribe<int>("value", [](int value) {
        EXPECT_EQ(100, value);
    });

    EXPECT_NO_THROW(oyoung::topic::publish("value", 100));
    EXPECT_ANY_THROW(oyoung::topic::publish("value", 100.0));

}