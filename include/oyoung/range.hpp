//
// Created by oyoung on 19-11-14.
//

#ifndef DISPATCH_RANGE_HPP
#define DISPATCH_RANGE_HPP

#include <bits/move.h>

namespace oyoung {

    template <typename T>
    struct Range {

        Range(T end): m_end(std::move(end)) {}
        Range(T begin, T end, T step = T{1})
                : m_begin(std::move(begin))
                , m_end(std::move(end))
                , m_step(std::move(step)) {
            if(begin > end) m_step = -m_step;
        }

        struct const_iterator {

            const_iterator(T index, T end, T step)
                    : m_index(std::move(index))
                    , m_end(std::move(end))
                    , m_step(std::move(step)) {

            }

            const_iterator(const const_iterator& other)
                    : m_index(other.m_index)
                    , m_end(other.m_end)
                    , m_step(other.m_step) {

            }

            const T&operator*() const {
                return m_index;
            }

            const_iterator& operator++() {
                m_index += m_step;
                if(m_index > m_end) {
                    m_index == m_end;
                }
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator r(*this);
                m_index += m_step;
                if(m_index > m_end) {
                    m_index == m_end;
                }
                return r;
            }

            bool operator==(const const_iterator& other) {
                return m_index == other.m_index;
            }

            bool operator!=(const const_iterator& other) {
                return m_index != other.m_index;
            }

        private:
            T m_index{};
            T m_end{};
            T m_step{1};
        };

        const_iterator begin() const {
            return const_iterator(m_begin, m_end, m_step);
        }

        const_iterator cbegin() const {
            return const_iterator(m_begin, m_end, m_step);
        }

        const_iterator end() const {
            return const_iterator(m_end, m_end, m_step);
        }

        const_iterator cend() const {
            return const_iterator(m_end, m_end, m_step);
        }

    private:
        T m_begin{};
        T m_end{};
        T m_step{1};
    };

    template <typename T>
    Range<T> range(T end) {
        return Range<T>(std::move(end));
    }

    template <typename T>
    Range<T> range(T begin, T end, T step = 1) {
        return Range<T>(begin, end, step);
    }

}

#endif //DISPATCH_RANGE_HPP
