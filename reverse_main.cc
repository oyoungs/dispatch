#include <oyoung/collection_reverse.hpp>

#include <gtest/gtest.h>


#include <map>
#include <list>
#include <vector>

TEST(oyoung_reverse, reverse_vector) {
    std::vector<int> vector {0, 1, 2, 3, 4, 5};
    int n = 5;
    for(auto i: oyoung::reverse(vector)) {
        EXPECT_EQ(i, n--);
    }
}

TEST(oyoung_reverse, reverse_list) {
    std::list<int> list {0, 1, 2, 3, 4, 5};
    int n = 5;
    for(auto i: oyoung::reverse(list)) {
        EXPECT_EQ(i, n--);
    }
}

TEST(oyoung_reverse, reverse_map) {
    std::map<int, int> map {
	    {0, 100}, {1, 101}, {2, 102}, {3, 103}, {4, 104}, {5, 105}};
    int key = 5;
    int val = 105;
    for(auto pair: oyoung::reverse(map)) {
        EXPECT_EQ(pair.first, key--);
	EXPECT_EQ(pair.second, val--);
    }
}
