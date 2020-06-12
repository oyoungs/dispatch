#include <oyoung/string_view.hpp>
#include <iostream>


void print_string_view(oyoung::string_view sv) {
    std::cout << sv << std::endl;
}

int main(int, char**) {

    oyoung::string_view sv1("This is the first string view");
    oyoung::string_view sv2 = std::string("This is the seconds string view");
    print_string_view(sv1);
    print_string_view(sv2);

    std::cout << "sv1 <==> sv2: " << sv1.compare(sv2) << std::endl;
    std::cout << "sv1[:11] <==> sv2: "  << sv1.compare(0, 11, sv2) << std::endl;
    std::cout << "sv1[:11] <==> sv2[:11]: " << sv1.compare(0, 11, sv2, 0, 11) << std::endl;

    std::cout << "sv1.at(1): " << sv1.at(1) << std::endl;
    std::cout << "sv2[8]: " << sv2[8] << std::endl;
    std::cout << "sv1.front(): " << sv1.front() << std::endl;
    std::cout << "sv2.back(): " << sv2.back() << std::endl;

    return 0;
}