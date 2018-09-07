#include <iostream>

#include <oyoung/dispatch.hpp>

int main(int argc, char *argv[]) try {

    auto  w = oyoung::promise<int>([=]() -> int {
        std::cout << "promise: 0" << std::endl;
        return 1;
    }).then<int>([](int n) -> int {
       std::cout << "promise: 1" << std::endl;
       return n + 1000;
    }).then<std::string>([](int n) -> std::string {
        std::cout << "promise: 2" << std::endl;
        return "number: " + std::to_string(n);
    });

    std::cout << w.get() << std::endl;

    return 0;
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}
