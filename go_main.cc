//
// Created by oyoung on 18-11-23.
//

#include <oyoung/go.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    auto channel = oyoung::make_channel<int>();

    go [&] {
        std::cout << "waiting for channel sending value" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        channel << 100;
    };

    int value = 0;
    channel >> value;

    std::cout << "channel value: " << value << std::endl;



    return 0;
}