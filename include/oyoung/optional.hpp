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
        optional(T value): _M_has(true), _M_value(value) {}

        optional(const optional&) = delete;
        optional(optional&& other)
                : _M_has(std::move(other._M_has))
                , _M_value(std::move(other._M_value)) {

        }

        bool is_null() const {
            return not _M_has;
        }

        bool operator==(decltype(nullptr)) const  {
            return is_null();
        }

        bool operator!=(decltype(nullptr)) const  {
            return not is_null();
        }

        bool let(T& value) const {
            return _M_has ? (value = _M_value, true): false;
        }

        optional&operator=(T value) {
            _M_has = true;
            _M_value = std::move(value);
            return *this;
        }

        void swap(optional& other) noexcept {
            std::swap(_M_has, other._M_has);
            std::swap(_M_value, other._M_value);
        }

        operator T() const {
            if(is_null()) {
                throw std::runtime_error("Nil of optional");
            }
            return _M_value;
        }

        void set() {
            _M_has = true;
        }

        void set(T value) {
            _M_has = true;
            _M_value = std::move(value);
        }

        void clear() {
            _M_has = false;
        }

        void assign(const T& value) {
            _M_has = true;
            _M_value = value;
        }

        T operator()(const T& def) {
            return is_null() ? def: _M_value;
        }



        bool operator!() const {
            return  is_null();
        }


        T& operator*() {
            if (not _M_has) {
                throw std::runtime_error("Nil of optional");
            }
            return _M_value;
        }

        const T& operator*() const {
            if (not _M_has) {
                throw std::runtime_error("Nil of optional");
            }
            return _M_value;
        }
    private:
        bool _M_has;
        T _M_value;
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
