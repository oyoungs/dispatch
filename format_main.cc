//
// Created by oyoung on 18-12-10.
//

#include <oyoung/format.hpp>
#include <iostream>

int main(int, char**)
{

    std::cout << oyoung::format("Hello, world, world, world!!!").replace("world", "C++").to_string() << std::endl;

    std::cout << oyoung::format("My name is %1, my age is %2, my gender is %3").arg("oyoung").arg(26).arg("male").to_string() << std::endl;

    return 0;
}