//
// Created by oyoung on 18-11-7.
//

#include <oyoung/optional.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    oyoung::optional<int> n;

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
        std::cout << n << std::endl;
    }

    int a = 0;
    if(n.let(a)) {
        std::cout << a << std::endl;
    }

    return 0;
}