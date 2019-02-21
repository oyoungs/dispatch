#include <oyoung/any.hpp>
#include <iostream>



int main(int argc, char **argv)
{

    /// use std::initializer_list ctor
    oyoung::any list {1, 2, 3, 4, 5, 6};

    list.emplace_back(0);

    for(auto n: list) {
        int i = n;
        std::cout << i << " ";
    }
    std::cout << std::endl;

    /// use ctor(const char *)
    oyoung::any value("Jack");

    /// use copy ctor
    std::cout << "---\nbefore copy, value: "  << oyoung::any_cast<std::string>(value) << std::endl;
    oyoung::any copied(value);
    std::cout << "copied value: " << oyoung::any_cast<std::string>(copied) << std::endl;
    std::cout << "after copied, value: "  << oyoung::any_cast<std::string>(value) << std::endl;

    /// use move ctor
    std::cout << "---\nbefore moved, value is null: " << std::boolalpha << value.is_null() << std::endl;
    oyoung::any moved(std::move(value));
    std::cout << "moved: " << oyoung::any_cast<std::string>(moved) << std::endl;
    std::cout << "after moved, value is null: " << std::boolalpha << value.is_null() << std::endl;


    std::string name {"Mikeray"};

    std::cout << "---\nbefore copy ctor called, name: " << name << std::endl;
    oyoung::any copied_name(name);
    std::cout << "after copy ctor called, name: " << name << std::endl;


    std::cout << "---\nbefore move ctor called, name: " << name << std::endl;
    oyoung::any moved_name(std::move(name));
    std::cout << "after move ctor called, name: " << name << std::endl;


    std::cout << "---\nbefore assign (const char *), value is null: " << value.is_null() << std::endl;
    value = "Lucy";
    std::cout << "after assign (const char *), value: " << oyoung::any_cast<std::string>(value) << std::endl;


    std::cout << "---\nbefore assign (T&&) [T = int], value: " << oyoung::any_cast<std::string>(value) << std::endl;
    value = 1000;
    std::cout << "after assign (T&&) [T = int], value: " << oyoung::any_cast<int>(value) << std::endl;

    name = "Naruto";

    std::cout << "---\nbefore assign (const T&) [T = std::string], value: " << oyoung::any_cast<int >(value) << std::endl;
    value = name;
    std::cout << "after assign (const T&) [T = std::string], value: " << oyoung::any_cast<std::string>(value) << std::endl;

    std::cout << "---\nbefore assign (nullptr), value is null: " << value.is_null() << std::endl;
    value = nullptr;
    std::cout << "after assign (nullptr), value is null: " << value.is_null() << std::endl;

    std::cout << "---\nbefore assign (T&&) [T = std::string], value is null: " << value.is_null() <<", name: " << name << std::endl;
    value = std::move(name);
    std::cout << "after assign (T&&) [T = std::string], value: " << oyoung::any_cast<std::string>(value) <<", name: " << name << std::endl;


    return 0;
}


