#ifndef OYOUNG_TOPIC_H
#define OYOUNG_TOPIC_H

#include <oyoung/any.hpp>

#include <vector>
#include <unordered_map>

namespace oyoung
{


namespace topic {

struct Manager {
  
    template<typename T, typename Func>
    void subscribe(const std::string& name, Func&& func) {
        std::function<void(const T&)> fun(std::forward<Func>(func));
        m_subscribers[name].emplace_back(fun);
    }
    
    template<typename T>
    void publish(const std::string& name, const T& value) {
        if (m_subscribers.find(name) == m_subscribers.end()) {
            return;
        }
        for(auto& subscriber: m_subscribers[name]) {
            auto func = any_cast<std::function<void(const T&)>>(subscriber);
            if (func) func(value);
        }
    }
    
    static Manager& shared() {
        static Manager instance {};
        return instance;
    }
    
private:
    Manager() = default;
    
private:
    std::map<std::string, std::vector<any>> m_subscribers;
};

template<typename T>
void publish(const std::string& name, const T& value) {
    Manager::shared().publish(name, value);
}

template< typename T, typename Func>
void subscribe(const std::string& name, Func&& func) {
    Manager::shared().subscribe<T>(name, std::forward<Func>(func));
}

}
} // namespace oyoung


#endif
