#include <iostream>

#include <oyoung/dispatch.hpp>
#include <ev++.h>
#include <nlohmann/json.hpp>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer, nlohmann::json>;

int main(int argc, char *argv[]) try {

    default_event_loop loop{};

    loop.on("start", [&loop](const default_event_loop::ev_data_t& data) {
        std::cout << "started" << std::endl;
        oyoung::dispatch(oyoung::get_global_queue(), [&loop] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            loop.emit("stop");
        });
    });

    loop.on("stop", [&loop](const default_event_loop::ev_data_t& data) {
        std::cout << "stopped" << std::endl;
        oyoung::dispatch(oyoung::get_global_queue(), [&loop] {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            loop.emit("start");
        });
    });


    loop.emit("start");


    return loop.exec();
} catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}
