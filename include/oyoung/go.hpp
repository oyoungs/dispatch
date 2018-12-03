//
// Created by oyoung on 18-11-23.
//

#ifndef DISPATCH_GO_HPP
#define DISPATCH_GO_HPP

#include <oyoung/dispatch.hpp>

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
namespace oyoung {
    template<typename T>
    struct channel {

        explicit channel(int buffer_size = 1)
                : q_max(buffer_size), q(std::make_shared<std::queue<T>>()) {
        }

        channel(const channel &other) : q_max(other.q_max), q(other.q) {}

        channel(channel &&other) : q_max(other.q_max), q(std::move(other.q)) { other.q_max = 0; }

        void set(const T& value) {
            std::unique_lock<std::mutex> lock(q_mutex);
            writable_or_close.wait(lock, [=] {
                return q->size() < q_max || !channel_open;
            });

            if (channel_open) {
                q->push(value);
                readable_or_close.notify_one();
            }
        }

        bool get(T& value) const {
            std::unique_lock<std::mutex> lock(q_mutex);
            readable_or_close.wait(lock, [=] {
                return q->size() > 0 || !channel_open;
            });
            if (!q->empty()) {
                value = q->front();
                q->pop();
                writable_or_close.notify_one();
                return true;
            }
            return false;
        }


        channel &operator<<(const T &value) {
            set(value);
            return *this;
        }

        channel &operator>>(T &value) {
            get(value);
            return *this;
        }

        void close() {
            channel_open = false;
            writable_or_close.notify_all();
            readable_or_close.notify_all();
        }

        ~channel() {
            close();
        }

    private:
        int q_max;
        bool channel_open{true};
        std::shared_ptr<std::queue<T>> q;
        mutable std::mutex q_mutex;
        mutable std::condition_variable readable_or_close, writable_or_close;
    };

    template <typename T>
    channel<T> make_channel(int buffer_size = 1) { return channel<T>(buffer_size); }

    struct _go_ {
        _go_() {
        }

        _go_ &operator+=(const std::function<void()> &func) {
            auto queue = oyoung::singleton<oyoung::concurrent_dispatch_queue>::only("GO");
            oyoung::async(*queue, func);
            return *this;
        }

    };


}

#define go oyoung::_go_() +=

#endif //DISPATCH_GO_HPP
