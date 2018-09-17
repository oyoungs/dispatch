# dispatch
Dispatch Tool for Modern C++ 

# Install

```bash
user@hostname: ~workdir$ mkdir -p build
user@hostname: ~workdir$ cd build
user@hostname: ~workdir$ cmake ..
user@hostname: ~workdir$ make install
```

# Usage


## 1. any
```c++
#include <oyoung/any.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    oyoung::any any {1, 2, 3, 4, 5, 6, 7};
    
    any.push_back(100);
    
    for(auto n: any) {
       std::cout << oyoung::any_cast<int>(n) << std::endl;
    }
    
    
    any = oyoung::any::object_t { {"name", "oyoung"}, {"age", 27} };
    
    any.insert("gender", "Male");
    
    for(auto it = any.begin(); it != any.end(); ++it) {
       auto value = it.value();
    
       std::cout << it.key() << ": ";
       if(value.is_string()) {
           std::cout << oyoung::any_cast<std::string>(value);
       } else if(value.is_number_integer()) {
           std::cout << oyoung::any_cast<int>(value);
       }
       std::cout       << std::endl;
    }  
    return 0;
}
```

## 2. dispatch

```c++
#include <oyoung/dispatch.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    auto queue = oyoung::dispatch_queue_create("name");

    oyoung::async(*queue, [=] {
       std::cout << "dispatch async calling" << std::endl;
    });

    std::cout << "dispatch sync will call" << std::endl;
    oyoung::sync(*queue, [=] {
        std::cout << "dispatch sync calling" << std::endl;
    });
    std::cout << "dispatch sync called" << std::endl;

    return 0;
}
```
## 3. promise


```c++
#include <oyoung/dispatch.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    auto task = oyoung::promise<int>([] {
        int result = 0;
        // do something to update the result;
        return result;
    }).then<int>([](int n /*last return value*/) {
        int another_result = 0;
        // do other things 
        return another_result;
    });
    try {
        std::cout << task.get() << std::endl;
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
      
    return 0;
}
```

## 4. event_loop


**example code:**

```c++
#include <oyoung/dispatch.hpp>
#include <ev++.h>
#include <iostream>

#include <nlohmann/json.hpp> // a beautiful only-header json toolkit 

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char **argv)
{
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
      
    return loop.exec();
}
```
Don't forget to add the "-lev" compile option
Note: 
1. cmake: add "target_link_libraries(target ev) to "CMakeLists.txt file

## 5. tcp server

**example code:**

```c++
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
```
