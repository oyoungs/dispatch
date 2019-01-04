//
// Created by oyoung on 18-11-7.
//

#include <oyoung/cli.hpp>
#include <iostream>

#include <nlohmann/json.hpp>



int main(int argc, char **argv)
{

    oyoung::cli::parse_options options;

    options.add("name", "--name", 0, "default");
    options.add("man", "--man", 'm', false);
    options.add("threads", "--threads", 'j', 10);

    try {
        auto result = oyoung::cli::parse(argc, argv, options);

        for(const auto& key: result.all_keys()) {
            std::cout << key << std::endl;
        }

        std::string name = result["name"];
        std::cout << "Name: " << name << std::endl;
        bool man = result["man"];
        std::cout << "Man: " << man << std::endl;
        oyoung::cli::options_integer thread = result["threads"];;
        std::cout << "threads: " << thread << std::endl;

    } catch (const std::exception& e) {
        std::cout << e.what();
    }

    return 0;
}