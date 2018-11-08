//
// Created by oyoung on 18-11-7.
//

#ifndef DISPATCH_CLI_HPP
#define DISPATCH_CLI_HPP

#include <oyoung/any.hpp>
#include <oyoung/utils.hpp>

#include <vector>
#include <typeinfo>


namespace oyoung {
    namespace cli {


        struct arguments
        {
            arguments() {}
            arguments(int argc, const char* const*argv): args(argv, argv + argc) {}

            std::string program() const {
                return args.front();
            }

            template <typename T>
            bool get(T& value, const std::string& long_name, char short_name, const T& def = {}) {

                std::string string{};
                std::string def_string = to_string(def);
                std::string type_name(typeid(T).name());

                if (!get_string(string, long_name, short_name, def_string)) {
                    return false;
                }


                return from_string(value, string);;
            }


            bool get_string(std::string& value, const std::string& long_name, char short_name, const std::string& def = "");

            bool get_boolean(bool& value, const std::string& long_name, char short_name);

        private:
            std::vector<std::string> args;
        };

        inline bool arguments::get_boolean(bool &value, const std::string &long_name, char short_name) {
            for(const auto& arg: args) {
                if(arg.find(long_name) != arg.npos) {
                    if(arg == long_name) {
                        value = true;
                    } else {
                        auto eq = arg.find('=');
                        if(eq != arg.npos) {
                            auto v = tolower(arg.substr(eq + 1));
                            if(v == "yes"  || v == "on" || v == "true") {
                                value = true;
                            } else if(v == "no"  || v == "off" || v == "false") {
                                value = false;
                            } else {
                                return false;
                            }
                        } else {
                            return false;
                        }
                    }
                    return true;
                }

                char sname[] = { '-', short_name, 0};
                if(arg == sname) {
                    value = true;
                    return true;
                }
            }
            return false;
        }

        inline bool arguments::get_string(std::string &value, const std::string &long_name, char short_name, const std::string& def) {
            char sname[] = {'-', short_name, 0};
            for(int i = 0, length = args.size(); i < length; ++i ) {
                const auto& arg = args.at(i);
                if(arg.find(long_name) != arg.npos) {
                    if(arg == long_name) {
                        if(i + 1 < length) {
                            const auto& next_arg = args.at(i + 1);
                            if(next_arg[0] != '-') {
                                value = next_arg;
                                return true;
                            } else {
                                value = def;
                                return true;
                            }
                        } else {
                            value = def;
                            return true;
                        }
                    } else {
                        auto eq = arg.find('=');
                        if(eq != arg.npos) {
                            value = arg.substr(eq + 1);
                            return true;
                        } else {
                            return false;
                        }
                    }

                }

                if(arg.find(sname) != arg.npos) {
                    if (arg == sname) {
                        if (i + 1 < length) {
                            const auto &next_arg = args.at(i + 1);
                            if (next_arg[0] != '-') {
                                value = next_arg;
                                return true;
                            } else {
                                value = def;
                                return true;
                            }
                        } else {
                            value = def;
                            return true;
                        }
                    } else {
                        value = arg.substr(2);
                        return true;
                    }
                }
            }
            return false;
        }

        using options = oyoung::any;
        using options_integer = long long;
        using options_unsigned = unsigned long long;
        using options_float = double;

        using parse_option = std::tuple<std::string, std::string, char, oyoung::any>;
        struct parse_options {
            using parse_option_list = std::vector<parse_option>;
            using iterator = parse_option_list::iterator;
            using const_iterator = parse_option_list::const_iterator;
            using reverse_iterator = parse_option_list::reverse_iterator;
            using const_reverse_iterator = parse_option_list::const_reverse_iterator;

            template <typename T>
            void add(const std::string& name, const std::string& long_name, char short_name, const T &def = {})
            {
                _parse_options.emplace_back(std::make_tuple(name, long_name, short_name, oyoung::any(def)));
            }

            iterator begin() noexcept
            {
                return _parse_options.begin();
            }

            iterator end() noexcept
            {
                return _parse_options.end();
            }

            const_iterator begin() const noexcept
            {
                return _parse_options.begin();
            }

            const_iterator end() const noexcept
            {
                return _parse_options.end();
            }

            const_iterator cbegin() const  noexcept
            {
                return _parse_options.cbegin();
            }

            const_iterator cend() const noexcept
            {
                return _parse_options.cend();
            }

            reverse_iterator rbegin() noexcept
            {
                return _parse_options.rbegin();
            }

            reverse_iterator  rend()  noexcept
            {
                return _parse_options.rend();
            }

            const_reverse_iterator crbegin() const  noexcept
            {
                return _parse_options.crbegin();
            }

            const_reverse_iterator crend() const noexcept
            {
                return _parse_options.crend();
            }


        private:
            parse_option_list _parse_options;
        };

        options parse(int argc, const char *const *argv, const parse_options& opts)
        {
            options result {};
            arguments args (argc, argv);
            for(const auto& opt: opts) {
                auto name = std::get<0>(opt);
                auto long_name = std::get<1>(opt);
                auto short_name = std::get<2>(opt);
                auto default_v = std::get<3>(opt);



                if(default_v.is_number_integer()) {
                    options_integer value {0};
                    if(default_v.type_name() == typeid(int).name()) {
                        value = (options_integer)any_cast<int>(default_v);
                    } else if(default_v.type_name() == typeid(short).name()) {
                        value = (options_integer)any_cast<short >(default_v);
                    } else if(default_v.type_name() == typeid(char).name()) {
                        value = (options_integer)any_cast<char >(default_v);
                    } else if(default_v.type_name() == typeid(long).name()) {
                        value = (options_integer)any_cast<long >(default_v);
                    } else if(default_v.type_name() == typeid(options_integer).name()) {
                        value = any_cast<options_integer>(default_v);
                    }

                    args.get(value, long_name, short_name, value);
                    result[name] = value;

                } else if(default_v.is_number_unsigned()) {
                    options_unsigned value {0};

                    if(default_v.type_name() == typeid(unsigned).name()) {
                        value = (options_unsigned)any_cast<unsigned >(default_v);
                    } else if(default_v.type_name() == typeid(unsigned short).name()) {
                        value = (options_unsigned)any_cast<unsigned short >(default_v);
                    } else if(default_v.type_name() == typeid(unsigned char).name()) {
                        value = (options_unsigned)any_cast<unsigned char >(default_v);
                    } else if(default_v.type_name() == typeid(unsigned long).name()) {
                        value = (options_unsigned)any_cast<unsigned long >(default_v);
                    } else if(default_v.type_name() == typeid(options_unsigned).name()) {
                        value = any_cast<options_unsigned>(default_v);
                    }
                    args.get(value, long_name, short_name, value);
                    result[name] = value;

                } else if(default_v.is_number_float()) {
                    options_float value {0};
                    if(default_v.type_name() == typeid(float).name()) {
                        value = (options_float) any_cast<float>(default_v);
                    } else if(default_v.type_name() == typeid(options_float).name()) {
                        value = any_cast<options_float >(default_v);
                    }

                    args.get(value, long_name, short_name, value);
                    result[name] = value;

                } else if(default_v.is_boolean()) {
                    bool value = default_v;
                    args.get_boolean(value, long_name, short_name);
                    result[name] = value;

                } else if(default_v.is_string()) {
                    std::string value  = default_v;
                    args.get_string(value, long_name, short_name, value);
                    result[name] = value;
                }
            }
            return  result;
        }
    }
}

#endif //DISPATCH_CLI_HPP
