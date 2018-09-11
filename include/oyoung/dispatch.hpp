/*
   ____  _____ _____ _____ _____ _____ _____ _   _
  |    \|_   _|   __|  _  |     |_   _|   __| |_| |  DISPATCH for Modern C++
  | |  | _| |_|__   |   __|  |  | | | |  |__|  _  |  version 1.0.0
  |____/|_____|_____|__|  |__|__| |_| |_____|_| |_|  https://github.com/Gitoyoung/dispatch

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  Copyright (c) 2018-?



 */

#ifndef OYOUNG_DISPATCH_HPP
#define OYOUNG_DISPATCH_HPP

/* all std includes */
#include <map>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <csignal>


/*all defines*/
// exclude unsupported compilers
#if defined(__clang__)
    #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
        #error "unsupported Clang version - see https://github.com/GitOyoung/dispatch#supported-compilers"
    #endif
#elif defined(__GNUC__) && !(defined(__ICC) || defined(__INTEL_COMPILER))
    #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
        #error "unsupported GCC version - see https://github.com/GitOyoung/dispatch#supported-compilers"
    #endif
#endif

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
#endif

// allow for portable deprecation warnings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define DISPATCH_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define DISPATCH_DEPRECATED __declspec(deprecated)
#else
    #define DISPATCH_DEPRECATED
#endif

// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && not defined(DISPATCH_NOEXCEPTION)
    #define DISPATCH_THROW(exception) throw exception
    #define DISPATCH_TRY try
    #define DISPATCH_CATCH(exception) catch(exception)
#else
    #define DISPATCH_THROW(exception) std::abort()
    #define DISPATCH_TRY if(true)
    #define DISPATCH_CATCH(exception) if(false)
#endif

// manual branch prediction
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define DISPATCH_LIKELY(x)      __builtin_expect(!!(x), 1)
    #define DISPATCH_UNLIKELY(x)    __builtin_expect(!!(x), 0)
#else
    #define DISPATCH_LIKELY(x)      x
    #define DISPATCH_UNLIKELY(x)    x
#endif

// cpp language standard detection
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
    #define DISPATCH_HAS_CPP_17
    #define DISPATCH_HAS_CPP_14
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
    #define DISPATCH_HAS_CPP_14
#endif

namespace oyoung {



template<std::size_t N>
struct dispatch_queue;

template<typename Q, typename Fn, typename ...Args>
void dispatch(Q& queue, Fn&& func, Args&& ...args)
{
   queue.dispatch(func, std::forward<Args>(args)...);
}


struct base_dispatch_queue
{
    bool running;
    std::string name;
    base_dispatch_queue(const std::string& name): name (name), running(false) {}
};

template<std::size_t N>
struct dispatch_queue: public base_dispatch_queue
{
    dispatch_queue(const std::string& name): base_dispatch_queue(name) {
        running = true;
        for(auto i = 0ul; i < N; ++i) {
            _internal_thread[i] = std::thread([=] {
                dispatch_on_new_thread();
            });
        }
    }

    template<typename Fn, typename ...Args>
    void dispatch(Fn && func, Args&& ...args) {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _task_queue.push(std::bind(func, std::forward<Args>(args)...));
        _queue_cv_runnable.notify_all();
    }

    ~dispatch_queue() {
        running = false;
        _queue_cv_runnable.notify_all();
        for(auto& thread: _internal_thread) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _queue_cv_empty.wait(lock, [=] { return _task_queue.empty();});
    }

private:
    void dispatch_on_new_thread() {
        while(running) {
            std::function<void()> func;
            {
                std::unique_lock<std::mutex> lock(_queue_mutex);
                _queue_cv_runnable.wait(lock, [=] { return !_task_queue.empty() || !running;});
                if(running) {
                    func = _task_queue.front();
                    _task_queue.pop();

                }
            }

            if(func) {
                func();
            }


            {
                std::unique_lock<std::mutex> lock(_queue_mutex);
                if(_task_queue.empty()) {
                    _queue_cv_empty.notify_all();
                }
            }
        }
    }

private:
    std::mutex  _queue_mutex;
    std::condition_variable _queue_cv_runnable;
    std::condition_variable _queue_cv_empty;
    std::thread _internal_thread[N];
    std::queue<std::function<void()>> _task_queue;
};

using serial_dispatch_queue = dispatch_queue<1ul>;

using concurrent_dispatch_queue = dispatch_queue<4ul>;

concurrent_dispatch_queue& get_global_queue()
{
    static concurrent_dispatch_queue global_queue("global");
    return global_queue;
}


template<typename T>
using promise_block = std::function<T()>;

template<typename T, typename U>
using promise_block_p = std::function<T(U)>;


template<typename T>
struct base_promise
{

    enum promise_status {
        ready, timeout, defered
    };
    base_promise(const promise_block<T>& block)
        : _queue(std::make_shared<serial_dispatch_queue>("promise")) {
        auto p = std::make_shared<std::promise<T> >();
        _future = p->get_future();
        _queue->dispatch( [=](const promise_block<T>& block) {
            try {
                T result = block();
                p->set_value(result);
            } catch(...) {
                try {
                    p->set_exception(std::current_exception());
                } catch(...) {}
            }
        }, block);
    }

    template<typename LastType>
    base_promise(base_promise<LastType>& from, const promise_block_p<T, LastType>& block)
        : _queue(from._queue) {

        auto last_future = std::make_shared<std::future<LastType>>(std::move(from._future));
        auto next_promise = std::make_shared<std::promise<T> >( );
        _future = next_promise->get_future();
        _queue->dispatch([=](const promise_block_p<T, LastType>& block) {
            try {
                LastType last = last_future->get();
                T next = block(last);
                next_promise->set_value(next);
            } catch(...) {
                try {
                    next_promise->set_exception(std::current_exception());
                } catch(...) {}
            }
        }, block);
    }

    base_promise(base_promise&& other)
        : _queue(other._queue), _future(std::move(other._future)) {
        other._queue.reset();
    }

    T get() {
        return _future.get();
    }

    void wait() {
        _future.wait();
    }

    template<typename Rep, typename Period>
    promise_status wait_for(const std::chrono::duration<Rep, Period>& duration) const {
        return (promise_status) _future.wait_for(duration);
    }

    template<typename Clock, typename Duration>
    promise_status wait_until(const std::chrono::time_point<Clock, Duration>& tp) {
       return (promise_status) _future.wait_until(tp);
    }

    template<typename NextType>
    base_promise<NextType> then(const promise_block_p<NextType, T>& block) {
        return base_promise<NextType>(*this, block);
    }




    std::shared_ptr<serial_dispatch_queue> _queue;
    std::future<T> _future;

};


template<typename T>
base_promise<T> promise(const promise_block<T>& block) {
    return base_promise<T>(block);
}

struct base_ev_loop
{
    thread_local static base_ev_loop *global;

    base_ev_loop() {
        global = this;

    }

    virtual void on(const std::string& name, const std::function<void(std::shared_ptr<void>)>& listener) = 0;
    virtual void emit(const std::string& name, std::shared_ptr<void> argument) = 0;
    virtual int exec() { return 0; }
    virtual ~base_ev_loop() {}
};

thread_local base_ev_loop* base_ev_loop::global = nullptr;
   


template <typename EV_LOOP, typename EV_IO, typename EV_ASYNC, typename EV_TIMER>
struct event_loop: public base_ev_loop
{
  using ev_loop_t = EV_LOOP;
  using ev_io_t = EV_IO;
  using ev_async_t = EV_ASYNC;
  using ev_timer_t = EV_TIMER;

  event_loop(const event_loop&) = delete;

  event_loop()
      : ev_code(0)
      , ev_loop(std::make_shared<ev_loop_t>())
      , ev_async(std::make_shared<ev_async_t>(*ev_loop))
  {
      ev_async->set(this);
      ev_async->start();
  }

  event_loop&operator=(const event_loop&) = delete;


  int exec() override
  {
      ev_loop->run(0);
      return ev_code;
  }

  void emit(const std::string& name, std::shared_ptr<void> argument) override
  {
      auto ev_item = std::make_tuple(name, argument);
      std::unique_lock<std::mutex> lock(queue_lock);
      queue.push(ev_item);
      ev_async->send();
      queue_not_empty.notify_one();
  }

  template<typename T>
  void emit(const std::string& name, const T & data = T{})
  {
      auto ev_data = std::make_shared<T>(std::move(data));
      emit(name, std::static_pointer_cast<void>(ev_data));

  }

  void on(const std::string &name, const std::function<void (std::shared_ptr<void>)> &listener) override
  {
     listeners[name].emplace_back(listener);
  }

  void start(int descriptor, int event)
  {
      auto ev_io = std::make_shared<ev_io_t>(*ev_loop);
      ev_io->set(this);
      ev_io->start(descriptor, event);
      ev_ios[descriptor] = ev_io;
  }

  void stop(int descriptor)
  {
      if(auto ev_io = ev_ios[descriptor]) {
          ev_io->stop();
      }

      ev_ios.erase(descriptor);
  }

  void break_loop()
  {
      if(ev_loop) {
          ev_loop->break_loop();
      }
  }

  template <typename Rep, typename Period>
  uint64_t set_timeout(const std::function<void()>& func, const std::chrono::duration<Rep, Period>& delay)
  {
      auto timer = std::make_shared<ev_timer_t >(ev_loop);
      timer->set(this);
      timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count() / 1000.0, 0);
      timers.emplace_back(std::make_tuple(timer, func));

      return reinterpret_cast<uint64_t >(timer.get());
  }

  void clear_timer(uint64_t id)
  {
      auto timer = reinterpret_cast<ev_timer_t *>(id);
      for (int i = 0, size = timers.size(); i < size; ++i) {
          auto ev_timer = std::get<0>(timers[i]);
          if(ev_timer.get() == timer) {
              ev_timer->stop();
              break;
          }
      }
  }

  template <typename Rep, typename Period>
  uint64_t set_interval(const std::function<void()>& func, const std::chrono::duration<Rep, Period>& repeat)
  {
      auto timer = std::make_shared<ev_timer_t >(ev_loop);
      timer->set(this);
      timer->start(0, std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count() / 1000.0);
      timers.emplace_back(std::make_tuple(timer, func));
      return reinterpret_cast<uint64_t >(timer.get());
  }


  ~event_loop() override {}

private:

  void operator()(ev_io_t & io, int event)
  {
      emit("io", {
          {"descriptor", io.fd},
          {"event", event}
      });
  }

  void operator()(ev_async_t & async, int event)
  {
      std::tuple<std::string, std::shared_ptr<void>> ev_item;
      {
          std::unique_lock<std::mutex> lock(queue_lock);
          queue_not_empty.wait(lock, [=] { return !queue.empty(); });
          if(!queue.empty()) {
              ev_item = queue.front();
              queue.pop();
          }
      }

      auto name = std::get<0>(ev_item);

      if(name.empty()) return;

      auto ev_data = std::get<1>(ev_item);

      if(ev_data) {
          for(auto func: listeners[name]) {
              func(ev_data);
          }
      }
  }

  void operator()(ev_timer_t & ev_timer, int event) {
      int select_timer_index = 0;
      for(auto i = 0ul, size = timers.size(); i < size; ++i) {
          auto  tuple = timers[i];
          auto  timer = std::get<0>(tuple);
          if(timer && &ev_timer == timer.get()) {
              auto func = std::get<1>(tuple);
              if(func) {
                  func();
              }

              select_timer_index = i;
              break;
          }
      }

      if(! ev_timer.is_active()) {
          timers.erase(timers.begin() + select_timer_index);
      }
  }




private:

  int           ev_code;
  std::shared_ptr<ev_loop_t>                    ev_loop;
  std::map<int, std::shared_ptr<ev_io_t>>       ev_ios;
  std::shared_ptr<ev_async_t>                   ev_async;

  std::vector<std::tuple<std::shared_ptr<ev_timer_t>, std::function<void()> > > timers;


  std::mutex queue_lock;
  std::condition_variable queue_not_empty;
  std::queue<std::tuple<std::string, std::shared_ptr<void> > > queue;
  std::map<std::string, std::vector<std::function<void(std::shared_ptr<void>)> > > listeners;
};

}


#endif
