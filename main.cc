#include <iostream>


#include <oyoung/net.hpp>
#include <oyoung/dispatch.hpp>
#include <ev++.h>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char *argv[]) /*try*/ {

    default_event_loop loop {};

    oyoung::net::tcp::default_server server("0.0.0.0", 9090);

    std::map<int, std::shared_ptr<oyoung::net::tcp::default_client>> clients;

    loop.on("io", [&](const oyoung::any& argument) {

        auto tuple = oyoung::any_cast<std::tuple<int, int>>(argument);

        auto descriptor = std::get<0>(tuple);

        if(descriptor == server.descriptor()) {

            auto client = server.accept(1);

            if(client) {
                std::cout << "new client: " << client->address() << ": " << client->port() << "(" << client->descriptor() << ")" << std::endl;

                loop.start(client->descriptor(), ev::READ);
                clients[client->descriptor()] = client;
            }

        } else {


            auto client = clients[descriptor];

            if(client) {

                auto length = client->bytes_available();

                auto result = client->read(length);

                if(result.success()) {
                    if(result.empty() ) {
                        std::cout << client->port() << ": " << std::string(result.begin(), result.end()) << std::endl;
                    } else {
                        std::cout << client->port() << ": " << std::string(result.begin(), result.end()) << std::flush;
                    }
                } else {
                    if(result.error() == oyoung::net::socket_error::connection_closed) {
                        loop.stop(descriptor);
                        std::cout << "client(" << client->port() << ", " << descriptor <<") is closed" << std::endl;
                    } else {
                        std::cout << "message: " << result.message() << std::endl;
                    }
                }
            }

        }

    });

    auto result = server.listen();

    if(result.success()) {
        loop.start(server.descriptor(), ev::READ);
    }

    loop.set_interval([] {
        std::cout << "loop is running" << std::endl;
    }, std::chrono::seconds(1));


    return loop.exec();

} /*catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}*/
