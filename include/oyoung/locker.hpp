//
// Created by oyoung on 19-11-14.
//

#ifndef DISPATCH_LOCKER_HPP
#define DISPATCH_LOCKER_HPP

#include <map>
#include <mutex>
#include <string>
#include <memory>

namespace oyoung {

    using Guard = std::lock_guard<std::mutex>;
    using MutexRef = std::shared_ptr<std::mutex>;

    struct MutexCache {


        MutexRef get(const std::string& name) {

            auto mutex = m_mutex_pool[name];

            if(mutex == nullptr) {
                mutex = std::make_shared<std::mutex>();
                m_mutex_pool.insert(std::make_pair(name, mutex));
            }

            return mutex;
        }

        static MutexCache& shared();

    private:
        std::map<std::string, MutexRef> m_mutex_pool;
    };

    inline MutexCache& MutexCache::shared() {
        static MutexCache sharedCache;
        return sharedCache;
    }

    inline MutexRef mutex(const std::string& name) {
        return MutexCache::shared().get(name);
    }

    struct Locker {

        Locker(const std::string& name)
                : m_mutex(mutex(name)) {

        }

        Locker&operator<<(const std::function<void()>& func) {
            {
                Guard guard(*m_mutex);
                if(func) func();
            }

            return *this;
        }

    private:
        MutexRef m_mutex;
    };

    using locker = Locker;

    template<typename Mutex>
    struct lock_guard: std::lock_guard<Mutex> {

        using super = std::lock_guard<Mutex>;

        explicit lock_guard(Mutex & mutex)
                : super(mutex) {

        }



        lock_guard&operator<<(const std::function<void()>& call) {

            if (call) call();

            return *this;
        }

    };
}

#ifndef lock
#define lock(NAME) oyoung::locker(NAME) <<
#endif

#ifndef with_lock
#define with_lock(mutex) oyoung::lock_guard<decltype(mutex)>(mutex) <<
#endif

#endif //DISPATCH_LOCKER_HPP
