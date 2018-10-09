//
// Created by oyoung on 10/8/18.
//

#ifndef DISPATCH_SINGLETON_HPP
#define DISPATCH_SINGLETON_HPP

#include <mutex>
#include <memory>

namespace oyoung
{
    template <typename T>
    struct singleton {

        template <typename ...Args>
                static std::shared_ptr<T> only(Args&& ...args)
        {
            std::lock_guard<std::mutex> guard(_lock);
            if(_one == nullptr) {
                _one = std::make_shared<T>(std::forward<Args>(args)...);
            }
            return _one;
        }

    private:
        static std::mutex _lock;
        static std::shared_ptr<T> _one;
    };

    template <typename T> std::mutex singleton<T>::_lock;
    template <typename T> std::shared_ptr<T> singleton<T>::_one;


    template <typename T>
    struct thread_local_singleton
    {
        template <typename ...Args>
        static std::shared_ptr<T> only(Args&& ...args)
        {
            std::lock_guard<std::mutex> guard(_lock);
            if(_one == nullptr) {
                _one = std::make_shared<T>(std::forward<Args>(args)...);
            }
            return _one;
        }

    private:
        thread_local static std::mutex _lock;
        thread_local static std::shared_ptr<T> _one;
    };

     template <typename T> thread_local std::mutex thread_local_singleton<T>::_lock;
     template <typename T> thread_local std::shared_ptr<T> thread_local_singleton<T>::_one;
}

#endif //DISPATCH_SINGLETON_HPP
