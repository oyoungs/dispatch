//
// Created by oyoung on 19-2-15.
//

#include <oyoung/property.hpp>
#include <iostream>


class Person {
public:
    oyoung::property<int> age{1};
    oyoung::property<std::string> gender{"Female"};
    oyoung::property<std::string> name {};
    oyoung::property<int> nextAge{};
    Person() {
        age.willSet([=] (int newVal) {
            return newVal > age;
        });
        age.didSet([=](int val, int old) {
            std::cout << "Age changed: ( current: " << val << ", old: " << old << ")" << std::endl;
        });
        name.bind(_name);

        nextAge.computed([=] {
            return age + 1;
        });
    }

    std::string& internal_name() { return _name; }

private:
    std::string _name {"Noname"};
};

int main(int, char**)
{

    Person person;

    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;


    /// assign property stored value
    person.age = 26;
    person.gender = "Male";


    /// assign bind member _name;
    person.name = "oyoung";

    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;

    /// assign age will be reject, because 22 less than the old age 26
    person.age = 22;
    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;

    std::cout << person.nextAge << std::endl;

    int next = person.nextAge;

    std::cout << next << std::endl;

    try {
        person.nextAge = 100;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}