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
    oyoung::property<std::string> full_name {};
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

        full_name.computed([=] {
            return _name + ".D.Luffy";
        });
    }

    std::string& internal_name() { return _name; }

private:
    std::string _name {"Noname"};
};

int main(int, char**)
{

    Person person;

    std::cout << std::boolalpha;

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


    person.age += 3;

    std::cout << person.nextAge << std::endl;

    int next = person.nextAge;

    std::cout << next << std::endl;

    try {
        person.nextAge = 100;
    } catch (const std::exception& e) {
        std::cout << "computed property nextAge= except: " << e.what() << std::endl;
    }

    try {
        person.full_name->c_str();
    } catch (const std::exception & e) {
        std::cout << "computed property fullName-> except: " << e.what() << std::endl;
    }


    person.name->append("123");

    std::cout << "after append, person's name is " << person.name << std::endl;

    std::cout << "person.age is between 0 and 100 ? " << (0 < person.age and person.age < 100) << std::endl;
    std::cout << "person.age is greater than 100 or equal to 0/100 or less than 0 ? " << (100 <= person.age or person.age <= 0) << std::endl;

    std::cout << "person.age == 29 ? " << (person.age == 29) << std::endl;
    std::cout << "person.age != 29 ? " << (person.age != 29) << std::endl;

    std::cout << "30 == person.age ? " << (30 == person.age) << std::endl;
    std::cout << "30 != person.age ? " << (30 != person.age) << std::endl;


    std::cout << "before 5 years, " << person.name << "'s age was " << (person.age - 5) << std::endl;
    std::cout << "after 5 years, " << person.name << "'s age will be " << (person.age + 5) << std::endl;

    std::cout << "person.age * 5: " << (person.age * 5) << std::endl;
    std::cout << "person.age / 5: " << (person.age / 5) << std::endl;

    std::cout << "person.age << 1: " << (person.age << 1) << std::endl;
    std::cout << "person.age >> 1: " << (person.age >> 1) << std::endl;

    return 0;
}