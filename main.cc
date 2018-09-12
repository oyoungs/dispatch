#include <iostream>

#include <oyoung/dispatch.hpp>
#include <ev++.h>
#include <nlohmann/json.hpp>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char *argv[]) /*try*/ {

    default_event_loop loop{};


    auto start = [=](const oyoung::any& argument) {
        auto arg = oyoung::any_cast<nlohmann::json> (argument);
        std::cout << arg.dump() << std::endl;
    };

    loop.on("start", start);

    loop.on("signal", start);

    loop.on("exception", [=] (const oyoung::any& argument) {
        auto what = oyoung::any_cast<std::string>(argument);
        std::cout << what << std::endl;
    });



    loop.emit("start", nlohmann::json{ {"name", "SB"} });

    auto signal_handle = [](int sig)->void {
        auto loop = dynamic_cast<default_event_loop *>(oyoung::base_ev_loop::global);
        loop->emit("signal", nlohmann::json {{"signal", sig}});
        loop->break_loop();
    };

    std::signal(SIGTERM, signal_handle);
    std::signal(SIGINT,signal_handle);
    std::signal(SIGKILL,signal_handle);



    return loop.exec();

} /*catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
}*/
