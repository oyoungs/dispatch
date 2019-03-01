//
// Created by oyoung on 19-3-1.
//

#include <oyoung/integer.hpp>
#include <iostream>

int main(int, char**)
{

    Int age(100);
    Double score(10);

    std::cout << "age: " << age << std::endl;

    age = 66.001;
    std::cout << "age: " << age << std::endl;

    Int number = age++;
    std::cout << "number: " << number << ", age: " << age << std::endl;

    score = Int(100) / 4;

    std::cout << "score: " << score << std::endl;

    std::cout << "score平方: " << score.square() << std::endl;
    std::cout << "score立方: " << score.cube() << std::endl;
    std::cout << "score平方根: " << score.sqrt() << std::endl;

    std::cout << "age + score: " << age + score << std::endl;
    std::cout << "age - score: " << age - score << std::endl;
    std::cout << "age * score: " << age * score << std::endl;
    std::cout << "age / score: " << age / score << std::endl;

    std::cout << "8 + age: " << 8 + age << std::endl;
    std::cout << "1.0 - age: " << 1.0 - age << std::endl;
    std::cout << "1.0 * age: " << 1.0 * age << std::endl;
    std::cout << "1000.0 / age: " << 1000.0 / age << std::endl;



    score = -0.5;
    std::cout << "score: " << score << std::endl;
    std::cout << "score's abs: " << score.abs() << std::endl;

    score *= 100;
    std::cout << "score *= 100, score: " << score << std::endl;

    Float temp;
    std::cout << "temp: " << temp << std::endl;


    if(age) {
        std::cout << "age != 0" << std::endl;
    }


    std::cout << std::chrono::duration_cast<std::chrono::seconds>(score.abs().hours()).count() << std::endl;

    return 0;
}