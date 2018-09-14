#include <oyoung/dispatch.hpp>
#include <iostream>


int main(int argc, char **argv)
{

    auto queue = oyoung::dispatch_queue_create("name");

    oyoung::async(*queue, [=] {
       std::cout << "dispatch async calling" << std::endl;
    });

    std::cout << "dispatch sync will call" << std::endl;
    oyoung::sync(*queue, [=] {
        std::cout << "dispatch sync calling" << std::endl;
    });
    std::cout << "dispatch sync called" << std::endl;




    return 0;
}
