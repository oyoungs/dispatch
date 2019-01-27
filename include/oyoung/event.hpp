//
// Created by oyoung on 18-11-16.
//

#ifndef DISPATCH_EVENT_HPP
#define DISPATCH_EVENT_HPP
/* oyoung includes*/
#include <oyoung/any.hpp>
#include <oyoung/singleton.hpp>
#include <oyoung/dispatch.hpp>

/* all std includes */

namespace oyoung {

    struct id_allocator
    {
        using id_type = std::size_t;

        id_type alloc()
        {
            for(id_type begin = 0, end = _allocated.size(); begin < end; ++begin) {
                if(not _allocated[begin]) {
                    _allocated[begin] = true;
                    return begin;
                }
            }

            _allocated.emplace_back(false);

            return _allocated.size() - 1;
        }

        void  dealloc(id_type id)
        {
            if(id < _allocated.size()) {
                _allocated[id] = false;
            }
        }

        static id_allocator& shared()
        {
            static id_allocator allocator;
            return allocator;
        }

    private:
        id_allocator() = default;
        std::vector<bool> _allocated;
    };

    namespace events {
        struct emitter {


            using id_type = id_allocator::id_type;
            using listener_type = std::function<void(const any &)>;
            using listener_tuple = std::tuple<id_type /*id*/, listener_type /*callback*/, bool /*once*/>;

            constexpr static id_type invalid_id {0xFFFFFFFFFFFFFFFFULL};

            void set_max_listeners(int count) {
                _max_listeners = count;
            }

            int max_listeners() const {
                return _max_listeners.is_null() ? default_max_listeners : any_cast<int>(_max_listeners);
            }


            void emit(const std::string &event, const any &arguments = {}) {
                auto found = _events.find(event);
                if(found == _events.end()) return;

                for(auto& tuple: found->second) {
                    auto id = std::get<0>(tuple);
                    auto func = std::get<1>(tuple);
                    auto once = std::get<2>(tuple);

                    std::exception_ptr eptr;
                    try {
                        if(func) func(arguments);
                    } catch (...) {
                        eptr = std::current_exception();
                    }

                    if (once) remove(event, id);

                    if(eptr) {
                        try {
                            std::rethrow_exception(eptr);
                        } catch (const std::exception& e) {
                            throw std::runtime_error(std::string("[") + event + "]" + e.what());
                        } catch (...) {
                            std::rethrow_exception(std::current_exception());
                        }
                    }

                }
            }

            /**
             * execute the listener when emit the event until the listener removed
             * @param event
             * @param listener
             * @return listener id
             */
            id_type on(const std::string &event, const listener_type &listener) {
                return append(event, listener, false);
            }

            /**
             * execute the listener only once when emit the event and auto remove itself
             * @param event
             * @param listener
             * @return
             */

            id_type once(const std::string &event, const listener_type &listener) {
                return append(event, listener, true);
            }

            id_type append(const std::string &event, const listener_type &listener, bool once = false) {
                if(listener_count() < max_listeners()) {
                    auto id = id_allocator::shared().alloc();
                    auto tuple = std::make_tuple(id, listener, once);
                    _events[event].emplace_back(tuple);
                    _events_count ++;
                    return id;
                }

                return invalid_id;
            }

            id_type prepend(const std::string &event, const listener_type &listener, bool once = false) {
                if(listener_count() < max_listeners()) {
                    auto id = id_allocator::shared().alloc();
                    auto tuple = std::make_tuple(id, listener, once);
                    _events[event].insert(_events[event].begin(), tuple);
                    _events_count ++;
                    return id;
                }
                return invalid_id;
            }

            void remove(const std::string &event, id_type id = invalid_id) {


                if(_events.find(event) == _events.end()) return;

                auto& events = _events[event];
                if(id ==  invalid_id) {
                    std::vector<listener_tuple> tuples;
                    tuples.swap(events);
                    for(auto& tuple: tuples) {
                        id_allocator::shared().dealloc(std::get<0>(tuple));
                    }
                    _events_count -= tuples.size();
                } else {
                    auto it = events.begin();
                    while (it != events.end()) {
                        if(std::get<0>(*it) == id) {
                            events.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    id_allocator::shared().dealloc(id);
                }


            }

            std::vector<listener_type> listeners() const {
                std::vector<listener_type> listener_list;

                for (const auto &pair: _events) {
                    for (const auto &tuple: pair.second) {
                        listener_list.emplace_back(std::get<1>(tuple));
                    }
                }

                return listener_list;
            }

            int listener_count() const {
                return _events_count;
            }

            constexpr static int default_max_listeners{10};


        private:
            struct Domain {
                any domain {};
                std::map<std::string, std::vector<listener_tuple>> _events;
                int _events_count {0};
                any _max_listeners {};
                any members {};
            } domain {};

            std::map<std::string, std::vector<listener_tuple>> _events;
            int _events_count {0};
            any _max_listeners {};
        };
    }



    struct base_ev_loop {

        base_ev_loop() {
            *thread_local_singleton<base_ev_loop *>::only() = this;
        }

        template<typename T>
        void emit(const std::string &name, const T &data = T{}) {
            emit(name, any(data));
        }

        virtual void on(const std::string &name, const std::function<void(const any &)> &listener) = 0;

        virtual void emit(const std::string &name, const any &argument = {}) = 0;

        virtual int exec() {
            std::this_thread::set_main_thread();
            return 0;
        }

        virtual ~base_ev_loop() {}
    };

    inline base_ev_loop * get_ev_loop_global()
    {
        return *thread_local_singleton<base_ev_loop *>::only();
    }


    template<typename EV_LOOP, typename EV_IO, typename EV_ASYNC, typename EV_TIMER>
    struct event_loop : public base_ev_loop {
        using ev_loop_t = EV_LOOP;
        using ev_io_t = EV_IO;
        using ev_async_t = EV_ASYNC;
        using ev_timer_t = EV_TIMER;

        event_loop(const event_loop &) = delete;

        event_loop(event_loop &&other)
                : ev_code(other.ev_code), ev_loop(std::move(other.ev_loop)), ev_async(std::move(other.ev_async)) {

        }

        event_loop()
                : ev_code(0), ev_loop(std::make_shared<ev_loop_t>()), ev_async(std::make_shared<ev_async_t>(*ev_loop)) {
            ev_async->set(this);
            ev_async->start();
        }

        event_loop &operator=(const event_loop &) = delete;


        int exec() override {
            ev_code = base_ev_loop::exec();
            ev_loop->run(0);
            return ev_code;
        }

        void emit(const std::string &name, const any &argument = {}) override {
            std::lock_guard<std::mutex> lock(queue_lock);
            queue.push(std::make_tuple(name, argument));
            ev_async->send();
        }

        void on(const std::string &name, const std::function<void(const any &)> &listener) override {
            listeners[name].emplace_back(listener);
        }

        void start(int descriptor, int event) {
            auto ev_io = std::make_shared<ev_io_t>(*ev_loop);
            ev_io->set(this);
            ev_io->start(descriptor, event);
            ev_ios[descriptor] = ev_io;
        }

        void stop(int descriptor) {
            if (auto ev_io = ev_ios[descriptor]) {
                ev_io->stop();
            }

            ev_ios.erase(descriptor);
        }

        void break_loop() {
            if (ev_loop) {
                ev_loop->break_loop();
            }
        }

        template<typename Rep, typename Period>
        uint64_t set_timeout(const std::function<void()> &func, const std::chrono::duration<Rep, Period> &delay) {
            auto timer = std::make_shared<ev_timer_t>(*ev_loop);
            timer->set(this);
            timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count() / 1000.0, 0);
            timers.emplace_back(std::make_tuple(timer, func));

            return reinterpret_cast<uint64_t >(timer.get());
        }

        void clear_timer(uint64_t id) {
            auto timer = reinterpret_cast<ev_timer_t *>(id);
            for (int i = 0, size = timers.size(); i < size; ++i) {
                auto ev_timer = std::get<0>(timers[i]);
                if (ev_timer.get() == timer) {
                    ev_timer->stop();
                    break;
                }
            }
        }

        template<typename Rep, typename Period>
        uint64_t set_interval(const std::function<void()> &func, const std::chrono::duration<Rep, Period> &repeat) {
            auto timer = std::make_shared<ev_timer_t>(*ev_loop);
            timer->set(this);
            timer->start(0, std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count() / 1000.0);
            timers.emplace_back(std::make_tuple(timer, func));
            return reinterpret_cast<uint64_t >(timer.get());
        }


        ~event_loop() override {}

    private:

        void operator()(ev_io_t &io, int event) {
            emit("io", std::make_tuple(io.fd, event));
        }

        void operator()(ev_async_t &async, int event) {

            decltype(queue) q;
            {
                std::lock_guard<std::mutex> lock(queue_lock);
                q.swap(queue);
            }

            while (!q.empty()) {
                auto ev_item = q.front();
                q.pop();


                auto name = std::get<0>(ev_item);

                if (name.empty()) continue;

                auto ev_data = std::get<1>(ev_item);

                for (auto func: listeners[name]) {
                    try {
                        func(ev_data);
                    } catch (const std::exception& e) {
                        emit("exception","[" + name + "] " +  e.what());
                    } catch (...) {
                        emit("exception","[" + name + "] unknown exception");
                    }
                }

            }
        }

        void operator()(ev_timer_t &ev_timer, int event) {
            int select_timer_index = 0;
            for (auto i = 0ul, size = timers.size(); i < size; ++i) {
                auto tuple = timers[i];
                auto timer = std::get<0>(tuple);
                if (timer && &ev_timer == timer.get()) {
                    auto func = std::get<1>(tuple);
                    if (func) {
                        try {
                            func();
                        } catch (const std::exception& e) {
                            emit("exception", e.what());
                        } catch (...) {
                            emit("exception", "unknown exception");
                        }
                    }

                    select_timer_index = i;
                    break;
                }
            }

            if (!ev_timer.is_active()) {
                timers.erase(timers.begin() + select_timer_index);
            }
        }


    private:

        int ev_code;
        std::shared_ptr<ev_loop_t> ev_loop;
        std::map<int, std::shared_ptr<ev_io_t>> ev_ios;
        std::shared_ptr<ev_async_t> ev_async;

        std::vector<std::tuple<std::shared_ptr<ev_timer_t>, std::function<void()> > > timers;


        std::mutex queue_lock;
        std::queue<std::tuple<std::string, any> > queue;
        std::map<std::string, std::vector<std::function<void(const any &)> > > listeners;
    };
}

#endif //DISPATCH_EVENT_HPP
