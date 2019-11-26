//
// Created by oyoung on 19-7-8.
//

#ifndef DISPATCH_DEFER_HPP
#define DISPATCH_DEFER_HPP

#include <vector>
#include <functional>

namespace oyoung {
    struct Defer {
        Defer& operator+=(std::function<void()> block) {
            m_defers.emplace_back(std::move(block));
            return *this;
        }

        ~Defer() {
            while (!m_defers.empty()) {
                if(m_defers.back()) m_defers.back()();
                m_defers.pop_back();
            }
        }

        void pop() {
            m_defers.pop_back();
        }
    private:
        std::vector<std::function<void()>> m_defers;
    };


}

#define USE_DEFER oyoung::Defer __defer__
#define defer __defer__ +=
#define defer_pop() __defer__.pop()

#endif //DISPATCH_DEFER_HPP
