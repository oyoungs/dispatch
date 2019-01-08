#include <oyoung/any.hpp>
#include <iostream>

int main(int argc, char **argv)
{

    oyoung::any any = oyoung::any::object_t { {"name", "oyoung"}, {"age", 27} };

    std::string name = any["name"];

    std::cout << name << std::endl;

    any.insert("gender", "Male");

    for(auto it = any.begin(); it != any.end(); ++it) {
        auto& value = it.value();

        std::cout << it.key() << ": ";
        if(value.is_string()) {
            std::cout << oyoung::any_cast<std::string>(value);
        } else if(value.is_number_integer()) {
            std::cout << oyoung::any_cast<int>(value);
        }
        std::cout << std::endl;
    }



    return 0;
}
