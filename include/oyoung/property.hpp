//
// Created by oyoung on 19-2-15.
//

#ifndef DISPATCH_PROPERTY_HPP
#define DISPATCH_PROPERTY_HPP

#include <memory>
#include <functional>

namespace oyoung {

    template <typename T>
    struct property {
        using value_type = typename std::remove_reference<T>::type;
        using const_value_type = const value_type;
        using reference_type = value_type &;
        using const_referennce_type =  const value_type &;

        using get_function = std::function<reference_type()>;
        using set_function = std::function<void(const_referennce_type)>;
        using will_set_function = std::function<bool(const_referennce_type)>;
        using did_set_function = std::function<void(const_referennce_type, const_referennce_type)>;

        property(const_referennce_type val)
                : _value(std::make_shared<value_type>(val)) {
            _get = [=] () -> reference_type { return ref() ;};
            _set = [=] (const_referennce_type other) {
                *_value = other;
            };
        }

        value_type value() const {
            return _computed ? _computed(): _value ? *_value : _get();
        }
        reference_type ref() {
            return _value ? *_value : _get();
        }
        const_referennce_type ref() const {
            return _value ? *_value : _get();
        }


        property() {}

        property(std::function<value_type()> computed)
                : _computed(std::move(computed)) {
            _get = [=] () -> reference_type {
                throw std::runtime_error("computed property contains no ref get function");
            };
            _will_set = [=] (const_referennce_type other) -> bool {
                throw std::runtime_error("computed property contains no set function");
            };
        }

        void computed(std::function<value_type()> computed) {
            _computed = std::move(computed);
            _get = [=] () -> reference_type {
                throw std::runtime_error("computed property contains no ref get function");
            };
            _will_set = [=] (const_referennce_type other) -> bool {
                throw std::runtime_error("computed property contains no set function");
            };

        }

        void bind(reference_type reference,
                  const will_set_function& check = nullptr,
                  const did_set_function& changed = nullptr) {

            _value.reset();
            _get = [&] () -> reference_type { return reference ;};
            _will_set = check;
            _set = [&] (const_referennce_type other) {
                reference = other;
            };
            _did_set = changed;
        }

        void assign(const_referennce_type val) {

            if(_will_set) {
                /// check new value is valid
                if(!_will_set(val)) return;
            }

            auto old = _get();
            auto changed = old != val;


            /// set new value
            if(_set) {
                _set(val);
            }

            /// value changed
            if(changed && _did_set) {
                _did_set(val, old);
            }
        }

        void swap(property<T>& other) {
            _value.swap(other._value);
            _get.swap(other._get);
            _will_set.swap(other._will_set);
            _set.swap(other._set);
            _did_set.swap(other._did_set);
        }

        void swap(reference_type reference) {
            std::swap(reference, ref());
        }

        property&operator=(const_referennce_type val) {
            assign(val);
            return *this;
        }

        operator value_type() const {
            return  _computed ? _computed(): _get ? _get() : value_type{};
        }


        void willSet(const will_set_function& function) {
            _will_set = function;
        }

        void didSet(const did_set_function& function) {
            _did_set = function;
        }

    private:
        get_function _get;
        will_set_function _will_set;
        set_function _set;
        did_set_function _did_set;

    private:
        std::function<value_type()> _computed{};
        std::shared_ptr<value_type> _value;
    };

    template <typename IStream, typename T>
    IStream& operator>>(IStream& in, property<T>& prop) {
        auto value = typename property<T>::value_type {};
        in >> value;
        prop = value;
        return in;
    };

    template <typename OStream, typename T>
    OStream& operator<<(OStream& out, const property<T>& prop) {
        out << prop.value();
        return out;
    };

}

#endif //DISPATCH_PROPERTY_HPP
