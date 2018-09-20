#include <iostream>


#include <oyoung/net.hpp>
#include <oyoung/dispatch.hpp>
#include <nlohmann/json.hpp>
#include <ev++.h>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char *argv[]) /*try*/ {

    default_event_loop loop {};

    oyoung::net::tcp::default_server<default_event_loop> server("0.0.0.0", 9090, loop);

    server.set_read_event(ev::READ);


    loop.on("accept", [&](const oyoung::any& argument) {
        auto client = oyoung::any_cast<std::shared_ptr<oyoung::net::tcp::default_client>>(argument);
        if(client) {
            std::cout << client->address() << ": " << client->port() << " connected" << std::endl;
            if(server.count() > 1) {
                std::cout << "before close, client is good? " << client->good() << std::endl;
                client->close();
                std::cout << "after close, client is good? " << client->good() << std::endl;
            }
        }
    });

    loop.on("data", [](const oyoung::any& argument) {
        std::cerr << "on data" << std::endl;
        using data_tuple = std::tuple<std::shared_ptr<oyoung::net::tcp::default_client>, std::shared_ptr<oyoung::net::Bytes>>;
        auto tuple = oyoung::any_cast<data_tuple>(argument);
        auto client = std::get<0>(tuple);
        auto bytes = std::get<1>(tuple);
        if(bytes) {
            std::cout << oyoung::net::String(bytes->begin(), bytes->end()) << std::flush;
            client->send(*bytes);
        } else {
            std::cout << "no data" << std::endl;
        }

    });

    loop.on("close", [](const oyoung::any& argument) {
       std::cout << oyoung::any_cast<std::shared_ptr<oyoung::net::tcp::default_client>>(argument)->port() << " closed" << std::endl;
    });

    loop.on("start", [&](const oyoung::any& argument) {
        server.start();
        std::cout << "server started" << std::endl;
    });


    loop.set_interval([&] {
        std::cout << "loop is running, client count: " << server.count() << std::endl;
    }, std::chrono::seconds(1));

    loop.emit("start");


    return loop.exec();

} /*catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}*/
