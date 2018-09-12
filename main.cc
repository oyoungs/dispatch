#include <iostream>


#include <oyoung/net.hpp>
#include <oyoung/dispatch.hpp>
#include <ev++.h>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char *argv[]) /*try*/ {

    default_event_loop loop {};

    oyoung::net::tcp::default_server<default_event_loop> server("0.0.0.0", 9090, loop);

    server.set_read_event(ev::READ);


    loop.on("accept", [=](const oyoung::any& argument) {
        auto client = oyoung::any_cast<std::shared_ptr<oyoung::net::tcp::default_client>>(argument);
        if(client) std::cout << client->address() << ": " << client->port() << " connected" << std::endl;
    });

    loop.on("data", [](const oyoung::any& argument) {
        std::cerr << "on data" << std::endl;

        auto bytes = oyoung::any_cast<std::shared_ptr<oyoung::net::Bytes>>(argument);
        if(bytes) {
            std::cout << oyoung::net::String(bytes->begin(), bytes->end()) << std::flush;
        } else {
            std::cout << "no data" << std::endl;
        }

    });

    loop.on("close", [](const oyoung::any& argument) {
       std::cout << oyoung::any_cast<std::shared_ptr<oyoung::net::tcp::default_client>>(argument)->descriptor() << " closed" << std::endl;
    });

    server.start();



    loop.set_interval([&] {
        std::cout << "loop is running, client count: " << server.count() << std::endl;
    }, std::chrono::seconds(1));


    return loop.exec();

} /*catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}*/
