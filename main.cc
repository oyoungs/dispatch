#include <iostream>

#include <oyoung/dispatch.hpp>

int main(int argc, char *argv[]) try {

    auto  p = oyoung::promise<int>([=]() -> int {
        std::cout << "promise: 0" << std::endl;
        return 1;
    });
    auto q = p.then<std::string>([=] (int n) -> std::string {
        std::cout << "promise: 1" << std::endl;
        return std::to_string(n + 1002);
    });
    auto w = q.then<int>([=] (std::string s) -> int {
        std::cout << "promise: 2" << std::endl;
        throw std::runtime_error("hehe");
        return 1000;
    });

//    w.wait();

    std::cout << w.get() << std::endl;

    return 0;
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}
