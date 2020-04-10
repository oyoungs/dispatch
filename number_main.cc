//
// Created by oyoung on 19-3-1.
//

#include <oyoung/number.hpp>
#include <iostream>
#include <complex>

using namespace oyoung::number;

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

    if(score < 120) {
        std::cout << "score < 120" << std::endl;
    }

    if(-100.0 < score) {
        std::cout << "score > -100.0" << std::endl;
    }

    std::cout << &score << std::endl;

    complex a;
    complex b;

    a = {5.0, 0.5};
    b = {1.0, 0.1};
    auto c = 1 - 0.5_i;

    std::cout << "complex a value: " << a << std::endl;
    std::cout << "complex b value: " << b << std::endl;
    std::cout << "complex c value: " << c << std::endl;
    std::cout << "complex a radius: " << a.radius() << std::endl;
    std::cout << "complex a theta: " << a.theta() << std::endl;
    std::cout << "complex a+b value: " << a+b << std::endl;
    std::cout << "complex a-b value: " << a-b << std::endl;
    std::cout << "complex a*b value: " << a*b << std::endl;
    std::cout << "complex a/b value: " << a/b << std::endl;
    std::cout << "complex a conjugate value: " << a.conjugate() << std::endl;

    return 0;
}