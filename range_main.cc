
#include <oyoung/range.hpp>
#include <iostream>

int main(int , char **) {

    std::cout << "range(9): ";
    for(auto i: oyoung::range(9)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(1, 10): ";
    for(auto i: oyoung::range(1, 10)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(20, 10): ";
    for(auto i: oyoung::range(20, 10)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(1, 10, step: 3): ";
    for(auto i: oyoung::range(1, 10, 3)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(20, 10, step: 2): ";
    for(auto i: oyoung::range(20, 10, 2)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    return 0;
}