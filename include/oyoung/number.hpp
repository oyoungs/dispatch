//
// Created by oyoung on 19-3-1.
//

#ifndef DISPATCH_INTEGER_HPP
#define DISPATCH_INTEGER_HPP

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <initializer_list>
#include <oyoung/format.hpp>

namespace oyoung {
    namespace number {

        template<typename T>
        struct is_supported_package {
            constexpr static bool value = false;
        };

        template<>
        struct is_supported_package<std::int8_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::uint8_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::int16_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::uint16_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::int32_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::uint32_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::int64_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<std::uint64_t> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<float> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<double> {
            constexpr static bool value = true;
        };
        template<>
        struct is_supported_package<long double> {
            constexpr static bool value = true;
        };

        template<typename T>
        struct type_package {

            static_assert(is_supported_package<T>::value, "Unsupported type");

            type_package() : _raw_value() {}

            type_package(const T &value) : _raw_value(value) {}

            type_package(const char *string) : _raw_value(std::strtold(string, nullptr)) {}

            template<typename U>
            type_package(const type_package<U> &other): _raw_value(static_cast<T>(other._raw_value)) {}

            type_package(const type_package &other) : _raw_value(other._raw_value) {}

            type_package(type_package &&other) : _raw_value(other._raw_value) {}


            template<typename U>
            type_package &operator=(const type_package<U> &other) {
                _raw_value = static_cast<T>(other._raw_value);
                return *this;
            }

            template<typename U>
            type_package &operator=(const U &value) {
                _raw_value = static_cast<T>(value);
                return *this;
            }

            T value() const {
                return _raw_value;
            }

            T &ref() {
                return _raw_value;
            }

            const T &ref() const {
                return _raw_value;
            }

            operator T() const {
                return _raw_value;
            }

            template<typename U>
            operator U() const {
                return static_cast<U>(_raw_value);
            }

            template<typename U>
            type_package<decltype(T() + U())> operator+(const type_package<U> &other) {
                return type_package<decltype(T() + U())>(_raw_value + other.ref());
            }

            template<typename U>
            type_package<decltype(T() - U())> operator-(const type_package<U> &other) {
                return type_package<decltype(T() - U())>(_raw_value - other.ref());
            }

            template<typename U>
            type_package<decltype(T() * U())> operator*(const type_package<U> &other) {
                return type_package<decltype(T() * U())>(_raw_value * other.ref());
            }

            template<typename U>
            type_package<decltype(T() / U())> operator/(const type_package<U> &other) {
                return type_package<decltype(T() / U())>(_raw_value / other.ref());
            }


            template<typename U>
            bool operator==(const type_package<U> &other) {
                return _raw_value == other.ref();
            }

            template<typename U>
            bool operator!=(const type_package<U> &other) {
                return _raw_value == other.ref();
            }

            template<typename U>
            bool operator<(const type_package<U> &other) {
                return _raw_value < other.ref();
            }

            template<typename U>
            bool operator<=(const type_package<U> &other) {
                return _raw_value <= other.ref();
            }

            template<typename U>
            bool operator>(const type_package<U> &other) {
                return _raw_value > other.ref();
            }

            template<typename U>
            bool operator>=(const type_package<U> &other) {
                return _raw_value >= other.ref();
            }

            type_package abs() const {
                return type_package(std::abs(_raw_value));
            }

            type_package square() const {
                return type_package(_raw_value * _raw_value);
            }

            type_package cube() const {
                return type_package(_raw_value * _raw_value * _raw_value);
            }

            type_package sqrt() const {
                return type_package(std::sqrt(_raw_value));
            }


            const T *operator&() const {
                return &_raw_value;
            }

            type_package &operator++() {
                ++_raw_value;
                return *this;
            }

            type_package operator++(int) {
                return type_package(_raw_value++);
            }

            type_package &operator--() {
                --_raw_value;
                return *this;
            }

            type_package operator--(int) {
                return type_package(_raw_value--);
            }

            type_package &operator+=(const type_package &other) {
                _raw_value += other.ref();
                return *this;
            }

            type_package &operator-=(const type_package &other) {
                _raw_value -= other.ref();
                return *this;
            }

            type_package &operator*=(const type_package &other) {
                _raw_value *= other.ref();
                return *this;
            }

            type_package &operator/=(const type_package &other) {
                _raw_value /= other.ref();
                return *this;
            }

            std::chrono::duration<T> hours() const {
                return std::chrono::duration<T, std::ratio<3600>>(_raw_value);
            }

            std::chrono::duration<T> minutes() const {
                return std::chrono::duration<T, std::ratio<60>>(_raw_value);
            }

            std::chrono::duration<T> seconds() const {
                return std::chrono::duration<T>(_raw_value);
            }

            std::chrono::duration<T> milliseconds() const {
                return std::chrono::duration<T, std::ratio<1, 1000>>(_raw_value);
            }

            std::chrono::duration<T> miscroseconds() const {
                return std::chrono::duration<T, std::ratio<1, 1000000L>>(_raw_value);
            }

            std::chrono::duration<T> nanoseconds() const {
                return std::chrono::duration<T, std::ratio<1, 10000000000L>>(_raw_value);
            }

            type_package<std::size_t> byte_size() const {
                return sizeof(_raw_value);
            }

            void swap(type_package& other) noexcept {
                std::swap(_raw_value, other._raw_value);
            }

//    T*operator&() {
//        return &_raw_value;
//    }
//    operator bool() const {
//        return _raw_value;
//    }
//


        protected:
            T _raw_value;
        };

        using Int8 = type_package<std::int8_t>;
        using Int16 = type_package<std::int8_t>;
        using Int32 = type_package<std::int8_t>;
        using Int64 = type_package<std::int8_t>;

        using UInt8 = type_package<std::uint8_t>;
        using UInt16 = type_package<std::uint8_t>;
        using UInt32 = type_package<std::uint8_t>;
        using UInt64 = type_package<std::uint8_t>;



        using Char = type_package<char>;
        using UChar = type_package<unsigned char>;
        using Byte = UChar;

        using Short = type_package<short>;
        using UShort = type_package<unsigned short>;

        using Int = type_package<int>;
        using UInt = type_package<unsigned int>;

        using Long = type_package<long>;
        using ULong = type_package<unsigned long>;

        using LLong = type_package<long long>;
        using ULLong = type_package<unsigned long long>;

        struct Float : type_package<float> {
            Float() : type_package() {}

            Float(float v) : type_package(v) {}

            Float &operator++() = delete;

            Float operator++(int) = delete;

            Float &operator=(float v) {
                _raw_value = v;
                return *this;
            }
        };

        struct Double : type_package<double> {
            Double() : type_package() {}

            Double(double v) : type_package(v) {}

            Double &operator++() = delete;

            Double operator++(int) = delete;

            Double &operator=(double v) {
                _raw_value = v;
                return *this;
            }
        };

        template<typename T, typename U>
        type_package<decltype(T() + U())> operator+(const T &value, const type_package<T> &package) {
            return type_package<decltype(T() + U())>(value + package.ref());
        };

        template<typename T, typename U>
        type_package<decltype(T() - U())> operator-(const T &value, const type_package<T> &package) {
            return type_package<decltype(T() - U())>(value - package.ref());
        };

        template<typename T, typename U>
        type_package<decltype(T() * U())> operator*(const T &value, const type_package<T> &package) {
            return type_package<decltype(T() * U())>(value * package.ref());
        };

        template<typename T, typename U>
        type_package<decltype(T() / U())> operator/(const T &value, const type_package<T> &package) {
            return type_package<decltype(T() / U())>(value / package.ref());
        };

        template<typename T, typename U>
        bool operator==(const T &lhs, const type_package<U> &rhs) {
            return lhs == rhs.ref();
        }

        template<typename T, typename U>
        bool operator!=(const T &lhs, const type_package<U> &rhs) {
            return lhs != rhs.ref();
        }

        template<typename T, typename U>
        bool operator<(const T &lhs, const type_package<U> &rhs) {
            return lhs < rhs.ref();
        }

        template<typename T, typename U>
        bool operator<=(const T &lhs, const type_package<U> &rhs) {
            return lhs <= rhs.ref();
        }

        template<typename T, typename U>
        bool operator>=(const T &lhs, const type_package<U> &rhs) {
            return lhs >= rhs.ref();
        }

        template<typename T, typename U>
        bool operator>(const T &lhs, const type_package<U> &rhs) {
            return lhs > rhs.ref();
        }

        template<typename OStream, typename T>
        OStream &operator<<(OStream &os, const type_package<T> &package) {
            return os << static_cast<T>(package);
        };


        struct complex {
            complex() : _real(0), _imag(0) {}

            complex(double value) : _real(value), _imag(0) {}

            complex(const Double &real, const Double &imag) : _real(real), _imag(imag) {}

            complex(const std::initializer_list<double>& list)
                    : _real(list.size() ? *list.begin(): 0.0)
                    , _imag(list.size() > 1 ? *(list.begin() + 1): 0.0) {

            }

            complex(const complex& other): _real(other._real), _imag(other._imag) {}
            complex(complex&& other): _real(std::move(other._real)), _imag(std::move(other._imag)) {}

            complex&operator=(const complex& other) {
                if(this != &other) {
                    complex(other).swap(*this);
                }
                return *this;
            }

            complex&operator=(complex&& other) {
                complex(std::move(other)).swap(*this);
                return *this;
            }

            complex&operator=(const std::initializer_list<double>& list) {

                complex(list).swap(*this);
                return *this;
            }

            Double radius() const {
                return (_real.square() + _imag.square()).sqrt();
            }

            Double theta() const {
                return std::atan2(_imag.ref(), _real.ref());
            }

            void swap(complex& other) noexcept {
                _real.swap(other._real);
                _imag.swap(other._imag);
            }

            complex operator+(const complex &other) const {
                return complex(_real + other._real, _imag + other._imag);
            }

            complex operator-(const complex &other) const {
                return complex(_real - other._real, _imag - other._imag);
            }

            complex operator*(const complex &other) const {
                auto real = _real * other._real - _imag * other._imag;
                auto image = _imag * other._real + _real * other._imag;
                return complex(real, image);
            }

            complex operator/(const complex &other) const {
                auto de = other._real.square() + other._imag.square();
                auto real = _real * other._real + _imag * other._imag;
                auto image = _imag * other._real - _real * other._imag;
                return complex(real / de, image / de);
            }

            complex conjugate() const {
                return complex(_real, -_imag);
            }

            Double &real() { return _real; }

            const Double &real() const { return _real; }

            Double &imag() { return _imag; }

            const Double &imag() const { return _imag; }

        protected:
            Double _real, _imag;
        };

        template<typename OStream>
        OStream &operator<<(OStream &os, const complex &comp) {
            const auto &real = comp.real();
            const auto &image = comp.imag();
            return os << oyoung::format("%1 %2 %3i")
                    .arg(real.ref())
                    .arg(image < 0 ? "-" : "+")
                    .arg(image.abs().ref()).to_string();
        }

    }
}



#endif //DISPATCH_INTEGER_HPP
