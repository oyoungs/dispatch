

#include <oyoung/locker.hpp>

#include <thread>
#include <iostream>

static int number = 0;

void increasement0();
void increasement1();
void decreasement0();
void decreasement1();

static std::mutex number_locker;

int main(int argc, char**argv) {

    auto sel = 1 < argc ? std::strtol(argv[1], nullptr, 10): 0;

    std::thread add, minus;

    switch (sel) {
        case 1: {
            add = std::thread(increasement1);
            minus  = std::thread(decreasement1);
        }
            break;
        default: {
            add = std::thread(increasement0);
            minus  = std::thread(decreasement0);
        }
            break;
    }


    if (add.joinable()) add.join();
    if (minus.joinable()) minus.join();
}

void increasement0() {

    int loop {100};
    while (loop --> 0) {
        lock("number") [=] {
            std::cout << "[+] after increase: " << ++number << std::endl;
        };
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}
void increasement1() {

    int loop {100};

    while (loop--) {
        with_lock(number_locker) [=] {
            std::cout << "[+] after increase: " << ++number << std::endl;
        };
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}

void decreasement0() {

    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lock("number") [=] {
            std::cout  << "[-] after decrease: " << --number << std::endl;
        };
    } while (number > 0);
}
void decreasement1() {
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        with_lock(number_locker) [=] {
            std::cout  << "[-] after decrease: " << --number << std::endl;
        };
    } while (number > 0);
}