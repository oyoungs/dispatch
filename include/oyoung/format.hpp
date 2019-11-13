//
// Created by oyoung on 18-12-10.
//

#ifndef DISPATCH_FORMAT_HPP
#define DISPATCH_FORMAT_HPP

#include <string>
#include <vector>
#include <sstream>

#include <chrono>
#include <iomanip>

namespace std {
    inline std::string to_string(double value, int precision) {
        std::ostringstream stream;
        stream.precision(precision);
        stream << std::fixed << value ;
        return stream.str();
    }
}

namespace oyoung {

    struct formatter {
        formatter(const std::string& format): _format_(format) {}

        template <typename T>
        formatter& arg(T value) {

            auto holder = current_place_holder();

            if(holder >= 0) {
                replace("%" + std::to_string(holder), std::to_string(value));
            }

            return *this;
        }

        formatter& arg(double value, int precision) {
            auto holder = current_place_holder();

            if(holder >= 0) {
                replace("%" + std::to_string(holder), std::to_string(value, precision));
            }

            return *this;
        }

        formatter& arg(const std::string& value) {
            auto holder = current_place_holder();

            if(holder >= 0) {
                replace("%" + std::to_string(holder), value);
            }


            return *this;
        }

        formatter& arg(const char * value) {
            auto holder = current_place_holder();

            if(holder >= 0) {
                replace("%" + std::to_string(holder), value);
            }

            return *this;
        }

        template <typename Clock, typename Duration>
        formatter& arg(const std::chrono::time_point<Clock, Duration>& time_point, const std::string& fmt) {
            auto holder = current_place_holder();
            auto value = time_format(time_point, fmt);

            if(holder >= 0) {
                replace("%" + std::to_string(holder), value);
            }

            return *this;
        }


        formatter& replace(const std::string& src, const std::string& target) {

            auto begin = _format_.find(src);
            while (begin != _format_.npos) {
                _format_.replace(begin, src.size(), target);
                begin = _format_.find(src);
            }

            return *this;
        }

        template <typename Iterator>
        std::string join(Iterator begin, Iterator end)
        {
            auto last = std::prev(end);
            auto joined = std::string {};

            if(std::distance(begin, last) <= 0) return joined;

            joined.reserve(256);

            for(auto it = begin; it != last; ++ it) {
                joined += *it;
                joined += _format_;
            }

            joined += *last;

            return joined;
        }

        template <typename Container>
        void split(const std::string& separator, Container& container) const {

            auto start = std::string::size_type {0u};
            auto fpos = _format_.find(separator, start);
            while (fpos != _format_.npos) {
                container.emplace_back(_format_.substr(start, fpos - start));
                start = fpos + separator.size();
                fpos = _format_.find(separator, start);
            }
            container.emplace_back(_format_.substr(start, fpos - start));
        }

        std::vector<std::string> split(const std::string& separator) const {
            std::vector<std::string> container {};
            split(separator, container);
            return container;
        }

        template <typename Container>
        Container split(const std::string& separator) const {
            Container container{};
            split(separator, container);
            return container;
        }

        template <typename Clock, typename Duration>
        static std::string time_format(const std::chrono::time_point<Clock, Duration>& time_point, const std::string& fmt)
        {
            auto time = Clock::to_time_t(time_point);
            auto puttime  = std::put_time(std::localtime(&time), fmt.c_str());
            auto ostream = std::ostringstream {};
            return (ostream << puttime, ostream).str();
        }

        std::string to_string() const  {
            return _format_;
        }

    private:

        int current_place_holder() const {
            int holder {-1};
            char *pos = nullptr;
            auto begin = _format_.c_str();
            auto end =  begin + _format_.size();
            while (begin < end) {
                if(begin[0] != '%') {
                    ++begin;
                    continue;
                }

                pos = nullptr;
                auto integer = std::strtol(begin + 1, &pos, 10);

                if(pos == nullptr || pos == begin) {
                    ++begin;
                    continue;
                }

                if(holder == -1 || integer < holder) {
                    holder = integer;
                }

                begin = pos;
            }
            return holder;
        }

    private:
        std::string _format_;
    };

    inline formatter format(const std::string& fmt) { return  formatter(fmt); }

    inline std::string substring(const std::string& src, std::string::size_type begin, std::string::size_type end = std::string::npos) {
        return src.substr(begin == std::string::npos ? 0: begin, end - begin);
    }

    inline std::string strip(const std::string& src, const std::string& chars = " \r\n\t") {
        auto tail = src.find_last_not_of(chars);
        return substring(src, src.find_first_not_of(chars), tail == src.npos ? tail: tail + 1);
    }

    inline std::string lstrip(const std::string& src, const std::string& chars = " \r\n\t") {
        return substring(src, src.find_first_not_of(chars));
    }

    inline std::string rstrip(const std::string& src, const std::string& chars = " \r\n\t") {
        auto tail = src.find_last_not_of(chars);
        return substring(src, 0, tail == src.npos ? tail: tail + 1);
    }
}

#endif //DISPATCH_FORMAT_HPP
