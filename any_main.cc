#include <oyoung/any.hpp>
#include <iostream>

int main(int argc, char **argv)
{

    oyoung::any any {1, 2, 3, 4, 5, 6, 7};

    any.push_back(100);

    for(auto n: any) {
        std::cout << oyoung::any_cast<int>(n) << std::endl;
    }


    any = oyoung::any::object_t { {"name", "oyoung"}, {"age", 27} };

    any.insert("gender", "Male");

    for(auto it = any.begin(); it != any.end(); ++it) {
        auto value = it.value();

        std::cout << it.key() << ": ";
        if(value.is_string()) {
            std::cout << oyoung::any_cast<std::string>(value);
        } else if(value.is_number_integer()) {
            std::cout << oyoung::any_cast<int>(value);
        }
        std::cout       << std::endl;
    }


    return 0;
}
