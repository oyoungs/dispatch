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
    oyoung::any any(0); // use constructor template<T> any(const T& v)
    
    any  = true;  // use <T> operator=(const T&)
    
    any  = "1234567890"; // use operator=(const char *)
    
    std::cout << oyoung::any_cast<std::string>(any) << std::endl; // may throw std::bad_cast exception
        
    return 0;
}
```

## 2. dispatch

```c++
#include <oyoung/dispatch.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    oyoung::dispatch(oyoung::get_global_queue(), []{
        std::cout << "dispatch callback" << std::endl;
    });
      
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
