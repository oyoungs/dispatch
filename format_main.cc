//
// Created by oyoung on 18-12-10.
//

#include <oyoung/format.hpp>
#include <iostream>

int main(int, char**)
{

    std::cout << oyoung::format("Hello, world, world, world!!!").replace("world", "C++").to_string() << std::endl;

    std::cout <<
              oyoung::format("My name is %1, my age is %2, my gender is %3, my height is %4m")
                      .arg("oyoung")
                      .arg(26)
                      .arg("male")
                      .arg(175.0, 1)
                      .to_string()
              << std::endl;

    std::cout << oyoung::formatter::time_format(std::chrono::system_clock::now(), "%Ec") << std::endl;

    std::cout << oyoung::format("Now: %1").arg(std::chrono::system_clock::now(), "%Y-%m-%d %H:%M:%S").to_string() << std::endl;

    return 0;
}