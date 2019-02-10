#pragma once

#include <vector>
#include <variant>
#include <string>
#include <unordered_map>
#include <functional>

class RPN
{
public:

    using Token = std::variant<std::string, double>;
    using TokenList = std::vector<Token>;
    using OperatorFn = std::function<double(double, double)>;
    using MapOperator = std::unordered_map<std::string, OperatorFn>;

    enum class SY_Associativity
    {
        LEFT,
        RIGHT,
        NONE
    };
    using SY_Precedence = int;
    using SY_Operator = std::tuple<SY_Precedence, SY_Associativity>;
    using SY_MapOperator = std::unordered_map<Token, SY_Operator>;

    static TokenList tokenFromString(const std::string &input);

    static double calculate(const std::string &input);
    static double calculate(const TokenList &list);
    static TokenList shuntingYardAlg(const TokenList &input);
    static TokenList shuntingYardAlg(const std::string &input);
    static double calculateShuntingYard(const std::string &input);

    static void registorOperator(const std::string &str, const OperatorFn &fn);
    static void registerSYOperator(const Token &token, const SY_Operator &op);
private:
    static MapOperator g_mapOperator;
    static SY_MapOperator g_SYMapOperator;
};