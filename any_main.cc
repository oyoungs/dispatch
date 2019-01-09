#include <oyoung/any.hpp>
#include <iostream>


#include <boost/any.hpp>

int main(int argc, char **argv)
{

    oyoung::any list {1, 2, 3, 4, 5, 6};

    list.emplace_back(0);

    for(auto n: list) {

        int i = n;
        std::cout << i << std::endl;
    }

    return 0;
}
