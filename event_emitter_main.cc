//
// Created by oyoung on 19-1-10.
//

#include <oyoung/event.hpp>
#include <iostream>

struct Test: oyoung::events::emitter {};

int main(int, char**)
{
    Test test;

    test.on("message", [=](const oyoung::any& any) {
        std::cout << "message" << std::endl;
    });

    test.emit("message");

    return 0;
}