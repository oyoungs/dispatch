//
// Created by oyoung on 18-11-7.
//

#include <oyoung/cli.hpp>
#include <iostream>

int main(int argc, char **argv)
{

    oyoung::cli::parse_options options;

    options.add("name", "--name", 'N', "default");
    options.add("man", "--man", 'M', false);

    try {
        auto result = oyoung::cli::parse(argc, argv, options);

        for(const auto& key: result.all_keys()) {
            std::cout << key << std::endl;

        }

        for(const auto& value: result) {
            if(value.is_boolean()) {
                std::cout << std::boolalpha << oyoung::any_cast<bool >(value) << std::endl;
            } else if(value.is_string()) {
                std::cout << oyoung::any_cast<std::string>(value) << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cout << e.what();
    }

    return 0;
}