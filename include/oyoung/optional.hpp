//
// Created by oyoung on 18-11-7.
//

#ifndef DISPATCH_OPTIONAL_HPP
#define DISPATCH_OPTIONAL_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace oyoung {
    template <typename T>
    struct optional {

        optional() {}
        optional(const T& value): v(std::make_shared<T>(value)) {}
        optional(T&& value): v(std::make_shared<T>(std::move(value))) {}

        optional(const optional& other): v(other.v) {}
        optional(optional&& other): v(std::move(other.v)) {}

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
                throw std::runtime_error("Null cannot be transform to value");
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

        T& operator()() {
            if(is_null()) {
                v = std::make_shared<T>();
            }
            return *v;
        }

        const T& operator()() const {
            static const T def {};
            return v ? *v : def;
        }

        bool operator!() const {
            return  is_null();
        }


        T& operator*() {
            if(is_null()) {
                v = std::make_shared<T>();
            }
            return *v;
        }

        const T& operator*() const {
            static const T def {};
            return v ? *v : def;
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


    template <typename T>
    using $ = optional<T>;

    using Int$ = $<int>;
    using Short$ = $<short>;
    using Long$ = $<long>;
    using LLong$ = $<long long >;
    using UInt$ = $<unsigned >;
    using UShort$ = $<unsigned short>;
    using ULong$ = $<unsigned long>;
    using ULLong$ = $<unsigned long long >;
    using Char$ = $<char>;
    using UChar$ = $<unsigned char>;
    using Float$ = $<float >;
    using Double$ = $<double>;
    using String$ = $<std::string>;



    template <typename K, typename V>
    using Dict = std::map<K, V>;

    template <typename K, typename V>
    using Dict$ = $<Dict<K,V> >;

    template <typename T>
    using Array = std::vector<T>;

    template <typename T>
    using Array$ = $<Array<T> >;


}



#endif //DISPATCH_OPTIONAL_HPP
