//
// Created by oyoung on 18-12-10.
//

#ifndef DISPATCH_FORMAT_HPP
#define DISPATCH_FORMAT_HPP

#include <string>

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



        formatter& replace(const std::string& src, const std::string& target) {

            auto begin = _format_.find(src);
            while (begin != _format_.npos) {
                _format_.replace(begin, src.size(), target);
                begin = _format_.find(src);
            }

            return *this;
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
}

#endif //DISPATCH_FORMAT_HPP
