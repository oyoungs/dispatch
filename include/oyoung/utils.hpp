//
// Created by oyoung on 18-11-7.
//

#ifndef DISPATCH_UTILS_HPP
#define DISPATCH_UTILS_HPP

#include <string>
#include <algorithm>
#include <stdexcept>
namespace oyoung {

    inline std::string tolower(const std::string& src)
    {
        std::string result(src);
        std::transform(src.begin(), src.end(), result.begin(), [=](const char & c) { return std::tolower(c); });
        return result;
    }

    inline bool from_string(bool& value, const std::string& string)
    {
        std::string lower(tolower(string));

        if(lower == "true" || lower == "t" || lower == "yes" || lower == "on") {
            value = true;
            return true;
        }

        if(lower == "false" || lower == "f" || lower == "no" || lower == "off") {
            value = false;
            return true;
        }

        throw std::invalid_argument("Can't transform string(\"" + string + "\") to boolean");
    }

    inline bool from_string(std::string& value, const std::string& string)
    {
        value = string;
        return true;
    }

    inline bool from_string(char& value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<char >(std::strtol(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(short &value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<short >(std::strtol(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(int & value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<int>(std::strtol(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(long& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtol(string.c_str(), &p, 10);
        return p && p != string.data();
    }

    inline bool from_string(long long& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtoll(string.c_str(), &p, 10);
        return p && p != string.data();
    }

    inline bool from_string(unsigned char& value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<unsigned char>(std::strtoul(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(unsigned short &value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<unsigned short >(std::strtoul(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(unsigned & value, const std::string& string)
    {
        char *p = nullptr;
        value = static_cast<unsigned >(std::strtoul(string.c_str(), &p, 10));
        return p && p != string.data();
    }

    inline bool from_string(unsigned long& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtoul(string.c_str(), &p, 10);
        return p && p != string.data();
    }

    inline bool from_string(unsigned long long& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtoull(string.c_str(), &p, 10);
        return p && p != string.data();
    }

    inline bool from_string(float& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtof(string.c_str(), &p);
        return p && p != string.data();
    }

    inline bool from_string(double& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtod(string.c_str(), &p);
        return p && p != string.data();
    }

    inline bool from_string(long double& value, const std::string& string)
    {
        char *p = nullptr;
        value = std::strtold(string.c_str(), &p);
        return p && p != string.data();
    }

    inline std::string to_string(char c)
    {
        return {c, 0};
    }

    inline std::string to_string(short number)
    {
        return std::to_string(number);
    }


    inline std::string to_string(int number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(long number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(long long number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(unsigned number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(unsigned long number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(unsigned long long number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(float number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(double number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(long double number)
    {
        return std::to_string(number);
    }

    inline std::string to_string(const char *string)
    {
        return std::string(string);
    }

    inline std::string to_string(const std::string& string)
    {
        return string;
    }

    inline std::string to_string(bool boolean)
    {
        return boolean ? "true" : "false";
    }









}

#endif //DISPATCH_UTILS_HPP
