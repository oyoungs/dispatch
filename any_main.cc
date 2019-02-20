#include <oyoung/any.hpp>
#include <iostream>



int main(int argc, char **argv)
{

    /// use std::initializer_list ctor
    oyoung::any list {1, 2, 3, 4, 5, 6};

    list.emplace_back(0);

    for(auto n: list) {

        int i = n;
        std::cout << i << std::endl;
    }

    /// use T&& ctor
    oyoung::any value(2019);

    /// use copy ctor
    std::cout << "before copy, value: "  << oyoung::any_cast<int>(value) << std::endl;
    oyoung::any copied(value);
    std::cout << "copied value: " << oyoung::any_cast<int>(copied) << std::endl;
    std::cout << "after copied, value: "  << oyoung::any_cast<int>(value) << std::endl;

    /// use move ctor
    std::cout << "before moved, copied is null: " << std::boolalpha << copied.is_null() << std::endl;
    oyoung::any moved(std::move(copied));
    std::cout << "moved: " << oyoung::any_cast<int>(moved) << std::endl;
    std::cout << "after moved, copied is null: " << std::boolalpha << copied.is_null() << std::endl;


    std::string name {"oyoung"};

    std::cout << "before copied, name: " << name << std::endl;
    oyoung::any copied_name(name);
    std::cout << "after copied, name: " << name << std::endl;


    std::cout << "before moved, name: " << name << std::endl;
    oyoung::any moved_name(std::move(name));
    std::cout << "after moved, name: " << name << std::endl;

    return 0;
}


