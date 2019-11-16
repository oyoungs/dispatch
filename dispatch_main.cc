#include <oyoung/event.hpp>
#include <iostream>


int main(int argc, char **argv)
{

    auto queue = oyoung::dispatch_queue_create("name");


    oyoung::events::emitter emitter;

    oyoung::async(*queue, [=] {
        std::cout << "dispatch async calling" << std::endl;
        std::cout << "dispatch thread: " << std::this_thread::get_id() << std::endl;

        std::cout << "dispatch sync will call" << std::endl;
        auto n = oyoung::sync(oyoung::dispatch_get_main_queue(), [=] {

            std::cout << "dispatch sync calling" << std::endl;
            if(std::this_thread::is_main_thread()) {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
            } else {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
            }

            return std::this_thread::get_id();
        });

        std::cout << "dispatch sync called, result: " << n << std::endl;
        std::cout << "dispatch main queue called" << std::endl;

        if(std::this_thread::is_main_thread()) {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
        } else {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
        }

    });


    return 0;
}
