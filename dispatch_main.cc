#include <oyoung/event.hpp>
#include <iostream>


struct RunLoop {


    void on(const std::string& event, const std::function<void(const oyoung::any&)>& func) {
        m_listeners[event].emplace_back(func);
    }

    void off(const std::string& event) {
        m_listeners[event].clear();
    }

    void emit(const std::string&event, const oyoung::any& args) {
        std::unique_lock<std::mutex> lock(m_task_mutex);
        m_events.emplace(std::make_tuple(event, args));
        m_task_not_empty.notify_all();
    }

    void stop(int user_code = 0) {
        m_user_code = user_code;
        m_running = false;
    }

    int exec() {

        m_running = true;

        while (m_running) {

            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_task_mutex);
                m_task_not_empty.wait(lock, [=] {
                    return !m_events.empty() || !m_running;
                });

                if(!m_events.empty()) {
                    auto event = m_events.front();
                    m_events.pop();

                    task = [=] {
                        for(const auto& func: m_listeners[std::get<0>(event)]) {
                            if(func) func(std::get<1>(event));
                        }
                    };
                }
            }

            if(task) {
                try {
                    task();
                } catch (const std::exception& e) {
                    emit("exception", e.what());
                }

            }
        }

        return m_user_code;
    }

private:
    bool m_running{false};
    int m_user_code{0};
    std::map<std::string, std::vector<std::function<void(const oyoung::any&)>>> m_listeners;
    std::mutex m_task_mutex;
    std::condition_variable m_task_not_empty;
    std::queue<std::tuple<std::string, oyoung::any>> m_events;
};

using MainQueue = oyoung::dispatch_main_queue<RunLoop>;

int main(int argc, char **argv)
{

    RunLoop loop{};

    auto queue = oyoung::dispatch_queue_create("name");


    oyoung::events::emitter emitter;

    MainQueue::set_dispatch_main_queue(std::make_shared<MainQueue >(loop));

    oyoung::async(*queue, [=] {
        std::cout << "dispatch async calling" << std::endl;
        std::cout << "dispatch thread: " << std::this_thread::get_id() << std::endl;

        std::cout << "dispatch sync will call" << std::endl;
        auto n = oyoung::sync(oyoung::dispatch_get_main_queue(), [=] {

            std::cout << "dispatch sync calling" << std::endl;
            if(std::this_thread::is_main_thread()) {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
            } else {
                std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
            }

            return std::this_thread::get_id();
        });

        std::cout << "dispatch sync called, result: " << n << std::endl;
        std::cout << "dispatch main queue called" << std::endl;

        if(std::this_thread::is_main_thread()) {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is  main thread" << std::endl;
        } else {
            std::cout << "dispatch thread(" << std::this_thread::get_id() <<") is not main thread" << std::endl;
        }

    });



    return loop.exec();
}
