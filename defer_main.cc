//
// Created by oyoung on 19-7-8.
//

#include <oyoung/defer.hpp>
#include <iostream>

struct Test {

    explicit Test(int n): m_number(new int(n)) {
        std::cout << "create number: " << n << std::endl;
        defer [=] {
            if(m_number) {
                std::cout << "release number: " << n << std::endl;
                delete  m_number;
                m_number = nullptr;
            }
        };
    }

    void clear() {

        std::cout << "clear number: " <<  (m_number ? std::to_string(*m_number): "null") << std::endl;

        delete  m_number;
        m_number = nullptr;
    }

private:
    USE_DEFER;
    int *m_number{nullptr};
};

int  main(int, char**) {

    USE_DEFER;

    std::cout << "create one" << std::endl;

    defer [=] {
        std::cout << "destroy one" << std::endl;
    };

    std::cout << "create two" << std::endl;

    defer [=] {
        std::cout << "destroy two" << std::endl;
    };

    Test test1(1);

    test1.clear();

    Test test2(2);

    defer_pop();

}