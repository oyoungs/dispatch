//
// Created by oyoung on 18-11-7.
//

#ifndef DISPATCH_OPTIONAL_HPP
#define DISPATCH_OPTIONAL_HPP

#include <memory>

namespace oyoung {
    template <typename T>
    struct optional {

        optional() {}
        optional(const T& value): v(std::make_shared<T>(value)) {}
        optional(T&& value): v(std::make_shared<T>(std::move(value))) {}

        optional(const optional& other): v(other.v) {}

        bool is_null() const {
            return v == nullptr;
        }

        bool operator==(decltype(nullptr)) const  {
            return is_null();
        }

        bool operator!=(decltype(nullptr)) const  {
            return !is_null();
        }

        bool let(T& value) const {
            return v ? (value = *v, true): false;
        }

        optional&operator=(const T&value) {
            if(v == nullptr || v.use_count() > 1) {
                v = std::make_shared<T>(value);
            } else {
                *v = value;
            }
            return *this;
        }

        optional&operator=(T&& value) {
            if(v == nullptr || v.use_count() > 1) {
                v = std::make_shared<T>(std::move(value));
            } else {
                *v = std::move(value);
            }
            return *this;
        }

        void swap(optional& other) noexcept {
            v.swap(other.v);
        }

        operator T() const {
            if(is_null()) {
                throw std::runtime_error("Null cannot be tranform to value");
            }
            return *v;
        }

        void clear() {
            v.reset();
        }

        void assign(const T& value) {
            *this = value;
        }

        T operator()(const T& def) {
            return is_null() ? def: *v;
        }


    private:
        std::shared_ptr<T> v;
    };

    template <typename T>
    bool operator==(decltype(nullptr), const optional<T>& opt)
    {
        return opt == nullptr;
    }

    template <typename T>
    bool operator!=(decltype(nullptr), const optional<T>& opt)
    {
        return opt != nullptr;
    }
}
#endif //DISPATCH_OPTIONAL_HPP
