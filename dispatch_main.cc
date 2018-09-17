#include <oyoung/dispatch.hpp>
#include <ev++.h>
#include <iostream>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;
using dispatch_main_queue_default = oyoung::dispatch_main_queue<default_event_loop >;

int main(int argc, char **argv)
{

    auto loop = default_event_loop {};
    auto queue = oyoung::dispatch_queue_create("name");

    dispatch_main_queue_default::set_dispatch_main_queue(std::make_shared<dispatch_main_queue_default >(loop));


    oyoung::async(*queue, [=] {
        std::cout << "dispatch async calling" << std::endl;
        std::cout << "dispatch thread: " << std::this_thread::get_id() << std::endl;

        std::cout << "dispatch sync will call" << std::endl;
        oyoung::sync(oyoung::dispatch_get_main_queue(), [=] {

            std::cout << "dispatch sync calling" << std::endl;
            if(std::this_thread::is_main_thread()) {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
            } else {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
            }
        });

        std::cout << "dispatch sync called" << std::endl;
        std::cout << "dispatch main queue called" << std::endl;

        if(std::this_thread::is_main_thread()) {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
        } else {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
        }

    });

    loop.on("exception", [=](const oyoung::any& argument) {
        auto what = oyoung::any_cast<std::string>(argument);
        std::cout << "exception: " << what << std::endl;
    });

    return loop.exec();
}
