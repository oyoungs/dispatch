

#include <oyoung/locker.hpp>

#include <thread>
#include <iostream>

static int number = 0;

void increasement();
void decreasement();


int main(int, char**) {

    std::thread add(increasement);
    std::thread minus(decreasement);

    add.join();
    minus.join();
}

void increasement() {

    int loop {100};
    while (loop --> 0) {
        lock("number") [=] {
            std::cout << "[+] after increase: " << ++number << std::endl;
        };
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}

void decreasement() {

    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lock("number") [=] {
            std::cout  << "[-] after decrease: " << --number << std::endl;
        };
    } while (number > 0);
}