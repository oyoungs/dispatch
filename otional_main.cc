//
// Created by oyoung on 18-11-7.
//

#include <oyoung/optional.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    oyoung::Int$ n;
    oyoung::Dict$<std::string, int> dict;

    if(n == nullptr) {
        std::cout << "N is null" << std::endl;
    }

    n = 100;

    if(nullptr != n) {
        std::cout << n << std::endl;
    }

    n.clear();

    n.assign(101);

    if(nullptr != n) {
        std::cout << n() << std::endl;
    }

    dict()["name"] = 1000;
    (!dict)["age"] = 200;
    (*dict)["number"] = 30;

    for(auto pair: !dict) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}