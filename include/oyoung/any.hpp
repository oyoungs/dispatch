/**
* any.hpp
*/

#ifndef OYOUNG_ANY_HPP
#define OYOUNG_ANY_HPP

#include <memory>
#include <string>
#include <iostream>
#include <typeinfo>

namespace oyoung
{
    struct holder
    {

        virtual std::shared_ptr<holder> clone() const = 0;
        virtual std::string type_name() const = 0;
        virtual ~holder() {}
    };

    template<typename T>
    struct place_holder: public holder
    {
        place_holder(const T &v) : value(v) {}

        std::string type_name() const override
        {
            return typeid(T).name();
        }

        std::shared_ptr<holder> clone() const override
        {
            return std::make_shared<place_holder>(value);
        }

        T value;
    };

    struct any
    {
        any() {}

        template<typename T>
        any(T&& value)
        : _holder(std::make_shared<place_holder<T>>(value)) {}
        
        any(const char *str): _holder(std::make_shared<place_holder<std::string>>(str)) {}
        
        any(const any& other): _holder(other._holder) {}
        
        any& operator=(const any& other) 
        {
            _holder = other._holder;
            return *this;
        }
        
        template<typename T>
        any& operator=(const T& val)
        {
            if(_holder == nullptr 
                || _holder->type_name() != typeid(T).name()
                || _holder.use_count() > 1) {
                _holder = std::make_shared<place_holder<T>>(val);
            } else {
            std::dynamic_pointer_cast<place_holder<T>>(_holder)->value = val;
            }
            return *this;
        }

        any& operator=(const char *str) 
        {        
            return *this = std::string(str);
        }
        
        any(any&& other)
            : _holder(other._holder) { other._holder.reset(); }
        
        template<typename T>
        T value(T&& def) const noexcept
        {
            if(_holder && typeid(T).name() == _holder->type_name()) {
                return std::dynamic_pointer_cast<place_holder<T>>(_holder)->value;
            } else {
                return def;
            }
        }

        std::string value(const char* def) const noexcept
        {
            return is_string() ? std::dynamic_pointer_cast<place_holder<std::string> >(_holder)->value : def;
        }


        bool is_string() const noexcept
        {
            return _holder && _holder->type_name() == typeid(std::string).name();
        }
        


        
        template<typename T>
        T value() const
        {
            if(_holder && typeid(T).name() == _holder->type_name()) {
                return std::dynamic_pointer_cast<place_holder<T>>(_holder)->value;
            } else {
                throw std::bad_cast();
            }
            
        }

        bool is_null() const noexcept
        {
            return _holder == nullptr;
        }

        bool is_number() const noexcept
        {
            return is_number_integer() || is_number_unsigned() || is_number_float();
        }

        bool is_number_integer() const noexcept
        {
            if(is_null()) return false;
            auto type = _holder->type_name();
            return type == typeid(char).name() 
                || type == typeid(short).name() 
                || type == typeid(int).name() 
                || type == typeid(long).name() 
                || type == typeid(long long).name();
        }

        bool is_number_unsigned() const noexcept
        {
            if(is_null()) return false;
            auto type = _holder->type_name();
            return type == typeid(unsigned short).name() 
                || type == typeid(unsigned char).name() 
                || type == typeid(unsigned).name() 
                || type == typeid(unsigned long).name() 
                || type == typeid(unsigned long long).name();
        }

        bool is_number_float() const noexcept
        {
            if(is_null()) return false;
            auto type = _holder->type_name();
            return type == typeid(float).name() 
                || type == typeid(double).name();
        }
        
    private:
        std::shared_ptr<holder> _holder;	
    };

    template<typename T>
    T any_cast(const any& a)
    {
        return a.value<T>();
    }
}

#endif // !OYOUNG_ANY_HPP