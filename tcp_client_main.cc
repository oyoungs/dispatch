//
// Created by oyoung on 9/18/18.
//
#include <oyoung/net.hpp>
#include <thread>
#include <iostream>

int main(int argc, char **argv)
{
    std::string address = argc < 2 ? "127.0.0.1": argv[1];
    int port    = argc < 3 ? 9090: std::strtol(argv[2], nullptr, 10);

    oyoung::net::tcp::default_client client(address, port);

    std::cout << "client info: \n  address: " << client.address() << "\n  port: " << client.port() << std::endl;

    auto result = client.connect(1);

    if(result.success()) {

        std::cout << "connect successfully" << std::endl;

        auto message = std::string(argc < 4 ? "This is a message for client test": argv[3]);
        auto sent = message.size();
        auto send_result = client.send(message);

        std::cout << "sent bytes(" << sent <<"):\n" << message << std::endl;

        while (client.bytes_available() == 0) {
            std::cout << "waiting..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        auto rresult = client.read(client.bytes_available());

        if (rresult.success()) {
            std::cout << "received: " << std::string(rresult.begin(), rresult.end()) << std::endl;
        }

    } else {
        std::cerr << "connect failed: " << result.message() << std::endl;
    }
    client.close();

    return 0;
}