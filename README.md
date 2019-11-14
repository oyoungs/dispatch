# dispatch
Dispatch Tool for Modern C++ 

# Install

```bash
~$ cmake -Bbuild -H.
~$ cmake --build build
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
#include <oyoung/event.hpp>
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

- Don't forget to add build dependency libev
```cmake
target_link_libraries(target ev)
```

**example code:**

```c++
#include <iostream>


#include <oyoung/net.hpp>
#include <nlohmann/json.hpp>
#include <ev++.h>

using default_event_loop = oyoung::event_loop<ev::default_loop, ev::io, ev::async, ev::timer>;

int main(int argc, char *argv[]) 
{

    default_event_loop loop {};

    oyoung::net::tcp::default_server server("0.0.0.0", 9090);

    server.set_read_event(ev::READ);

    server.on("start", [=, &loop](const oyoung::any& arguments) {
        auto tuple = oyoung::any_cast<std::tuple<int, int>>(arguments);
        loop.start(std::get<0>(tuple), std::get<1>(tuple));
    });

    server.on("stop", [=, &loop](const oyoung::any& arguments) {
        auto descriptor = oyoung::any_cast<int>(arguments);
        loop.stop(descriptor);
    });

    server.on("accept", [=, &loop](const oyoung::any& arguments) {
       loop.emit("accept", arguments);
    });

    server.on("data", [=, &loop](const oyoung::any& arguments) {
       loop.emit("data", arguments);
    });

    server.on("close", [=, &loop](const oyoung::any& arguments) {
       loop.emit("close", arguments);
    });

    loop.on("io", [=, &server](const oyoung::any& arguments) {
       server.emit("io", arguments);
    });

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

}
```

### 6. cli (commond line parser)
```cpp
#include <oyoung/cli.hpp>
#include <iostream>

int main(int argc, char **argv)
{

    /* parse command line options*/
    oyoung::cli::parse_options options; 

    /* user arguments*/
    options.add("name", "--name", 'N', "default"); // --name=<NAME> OR --name NAME OR -N NAME
    options.add("man", "--man", 'M', false); // --man OR -M OR --man=<BOOL> OR --man/-M <BOOL>, <BOOL> may be ON/OFF/on/off/YES/NO/yes/no/T/F

    try {
        auto result = oyoung::cli::parse(argc, argv, options); // result will be { "name": "...", "man": true/false }        
        

        for(const auto& key: result.all_keys()) {
            std::cout << key << std::endl;
        }

        for(const auto& value: result) {
            if(value.is_boolean()) {
                std::cout << std::boolalpha << oyoung::any_cast<bool >(value) << std::endl;
            } else if(value.is_string()) {
                std::cout << oyoung::any_cast<std::string>(value) << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cout << e.what();
    }

    return 0;
}
```
### 7. optional 
```cpp
#include <oyoung/optional.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    oyoung::Int$ n;
    oyoung::Dict$<std::string, int> dict;
    


    /* compare with nullptr */
    if(n == nullptr) {
        std::cout << "N is null" << std::endl;
    }

    /* assign with operator= */
    n = 100;

    /* compare with nullptr */
    if(nullptr != n) {
    
        /* auto use operator int()*/
        std::cout << n << std::endl;
    }

    /*clear value to null*/
    n.clear();

    /* use the internal value or the default value given by operator() if null*/
    std::cout << n(1000) << std::endl;
    

    /* assign*/
    n.assign(101);

    if(nullptr != n) {
    
        std::cout << n << std::endl;
    }

    int a = 0;
    
    /* use let to get value, and condition will be false if value is null */
    if(n.let(a)) {
        std::cout << a << std::endl;
    }
    
     dict()["name"] = 1000;
    (*dict)["age"] = 200;
    (*dict)["number"] = 30;

    for(auto pair: *dict) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
```

### 8. format
```cpp

#include <oyoung/format.hpp>
#include <iostream>

int main(int, char**)
{

    std::cout << oyoung::format("Hello, world, world, world!!!").replace("world", "C++").to_string() << std::endl;

    std::cout <<
              oyoung::format("My name is %1, my age is %2, my gender is %3, my height is %4m")
                      .arg("oyoung")
                      .arg(26)
                      .arg("male")
                      .arg(175.0, 1)
                      .to_string()
              << std::endl;

    std::cout << oyoung::formatter::time_format(std::chrono::system_clock::now(), "%Ec") << std::endl;

    std::cout << oyoung::format("Now: %1").arg(std::chrono::system_clock::now(), "%Y-%m-%d %H:%M:%S").to_string() << std::endl;

    return 0;
}


```

### 9. property
property.hpp provides some api of class/struct property, including set, get, willSet, didSet ...

Usage:
```cpp
#include <oyoung/property.hpp>
#include <iostream>


class Person {
public:
    oyoung::property<int> age{1};
    oyoung::property<std::string> gender{"Female"};
    oyoung::property<std::string> name {};
    Person() {
        age.willSet([=] (int newVal) {
            return newVal > age;
        });
        age.didSet([=](int val, int old) {
            std::cout << "Age changed: ( current: " << val << ", old: " << old << ")" << std::endl;
        });
        name.bind(_name);
    }

    std::string& internal_name() { return _name; }

private:
    std::string _name {"Noname"};
};

int main(int, char**)
{

    Person person;

    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;


    /// assign property stored value
    person.age = 26;
    person.gender = "Male";


    /// assign bind member _name;
    person.name = "oyoung";

    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;

    /// assign age will be reject, because 22 less than the old age 26
    person.age = 22;
    std::cout << person.internal_name() << ", gender: " << person.gender << ", age: " << person.age << std::endl;


    return 0;
}
```

### 10. simple lock

```cpp


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
```

### 11. range
```cpp

#include <oyoung/range.hpp>
#include <iostream>

int main(int , char **) {

    std::cout << "range(9): ";
    for(auto i: oyoung::range(9)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(1, 10): ";
    for(auto i: oyoung::range(1, 10)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(20, 10): ";
    for(auto i: oyoung::range(20, 10)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(1, 10, step: 3): ";
    for(auto i: oyoung::range(1, 10, 3)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "range(20, 10, step: 2): ";
    for(auto i: oyoung::range(20, 10, 2)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    repeat(5) [](int i) {
        std::cout << "repeat " << i << " time" << std::endl;
    };

    int times = 0;
    repeat(3) [&] {
        std::cout << "repeat " << times++ << std::endl;
    };

    return 0;
}
```
