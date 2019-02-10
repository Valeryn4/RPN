# RPN
Shunting-yard algorithm and Reverse Polish notation calculate

Это калькулятор, сделанны на Алгоритме сортировочной станции.

```c++

#include <iostream>
#include <cassert>
#include "RPN.h"

int main()
{

    std::string test1 = "2 + 2 * 2"; //6
    std::string test2 = "2 * 100 / 2"; //100
    std::string test3 = "2 + (2 - 3) * (2 + 1)"; //-1
    std::string test4 = "(-100 + -100) / 2"; //-100

    auto res1 = RPN::calculateShuntingYard(test1);
    auto res2 = RPN::calculateShuntingYard(test2);
    auto res3 = RPN::calculateShuntingYard(test3);
    auto res4 = RPN::calculateShuntingYard(test4);
    std::cout << "res 1: " << res1 << std::endl;
    std::cout << "res 2: " << res2 << std::endl;
    std::cout << "res 3: " << res3 << std::endl;
    std::cout << "res 4: " << res4 << std::endl;

    assert(res1 == 6.0);
    assert(res2 == 100.0);
    assert(res3 == -1.0);
    assert(res4 == -100.0);


    system("pause");
    return 0;
}
```

Так же есть две функции, позволяющие регистрировать свои операторы
```c++
    RPN::registorOperator(const std::string &str, const OperatorFn &fn);
    RPN::registerSYOperator(const Token &token, const SY_Operator &op);
```

Пример
```c++
    std::string test = "sum(100, 100) - 50";
    //                     name        function
    RPN::registorOperator("sum", [](double a, double b) { return a + b; }); //<-- это необходимо, для RPN самого калькулятора
    //                       name    predicate                assoc
    RPN::registerSYOperator("sum", { 2, RPN::SY_Associativity::FUNCTION }); //<-- это регистрация токена для алгоритма

    auto res = RPN::calculateShuntingYard(test);

    std::cout << "res: " << res << std::endl;
```
