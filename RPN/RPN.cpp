#include "RPN.h"

#include <stack>
#include <exception>

RPN::MapOperator RPN::g_mapOperator = 
{
    {"+", [](double a, double b) -> double {return a + b; }},
    {"-", [](double a, double b) -> double {return a - b; }},
    {"*", [](double a, double b) -> double {return a * b; }},
    {"^", [](double a, double b) -> double {return std::pow(a, b); }},
    {"/", [](double a, double b) -> double
        { 
            if (b == 0.0) 
                throw std::domain_error("operator '/' arg b is ZERO"); 
            return a / b;
        }
    },
};

using Assoc = RPN::SY_Associativity;

RPN::SY_MapOperator RPN::g_SYMapOperator =
{
    {"+", {2, Assoc::LEFT}},
    {"-", {2, Assoc::LEFT}},
    {"*", {3, Assoc::LEFT}},
    {"^", {4, Assoc::RIGHT}},
    {"/", {3, Assoc::LEFT}}
};

RPN::TokenList RPN::tokenFromString(const std::string & input)
{

    std::string num;
    std::string op; //<-- не стандартные/опциональные операторы
    bool isMinus = false;
    TokenList list;

    //запись кастомного оператора
    auto completeOperator = [&op, &list]()
    {
        if (!op.empty())
        {
            list.push_back(op);
            op.clear();
        }
    };

    //запись числа
    auto completeNumber = [&num, &list]()
    {
        if (!num.empty())
        {
            list.push_back(std::atof(num.c_str()));
            num.clear();
        }
    };

    //проверка на число
    auto isNum = [](char ch) 
    {
        return (ch >= '0' && ch <= '9' || ch == '.');
    };
    //for (auto ch : input)
    for (int i = 0; i < input.length(); ++i)
    {
        auto ch = input[i];
        //добавляем цифры
        if (isNum(ch))
        {
            //записываем кастомный оператор, если началось число
            //TODO если имя оператора содержит цифры?
            completeOperator();

            num += ch;
            continue;
        }

        //если число закончилось
        completeNumber();

        //условный разделитель для цифр и операторов
        if (ch == ' ')
        {
            //записываем кастомный оператор, если появился разделитель
            completeOperator();

            continue;
        }

        if (ch == '-' && i + 1 < input.length())
        {
            if (isNum(input[i + 1]))
            {
                num += ch;
                continue;
            }
        }

        //default operator
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^')
        {
            //записывает кастомный оператор, если начался стандартный оператор
            completeOperator();

            list.push_back(std::string(1, ch));
            continue;
        }

        //если это скобки или запятая
        if (ch == ')' || ch == '(' || ch == ',')
        {
            completeOperator();

            list.push_back(std::string(1, ch));
            continue;
        }
        //function
        op += ch;
    }
    //завершаем оператор
    completeOperator();
    completeNumber();

    return list;
}

//parsing string
double RPN::calculate(const std::string & input)
{
    return calculate(tokenFromString(input));
}

double RPN::calculate(const TokenList & list)
{
    std::stack<double> stak;

    for (auto &&v : list)
    {
        bool isNum = std::get_if<double>(&v);
        if (isNum)
        {
            stak.push(std::get<double>(v));
            continue;
        }

        if (std::get<std::string>(v) == ",")
            continue;

        if (stak.size() < 2)
            throw std::domain_error("unexepted error");
        
        std::string op = std::get<std::string>(v);

        double n2 = stak.top(); stak.pop();
        double n1 = stak.top(); stak.pop();
        double res = g_mapOperator[op](n1, n2);
        stak.push(res);
    }

    return stak.top();
}

RPN::TokenList RPN::shuntingYardAlg(const TokenList & input)
{
    TokenList out;
    std::stack<Token> stak;

    for (auto &&token : input)
    {
        if (std::get_if<double>(&token))
        {
            out.push_back(token);
            continue;
        }

        auto it_op1 = g_SYMapOperator.find(token);
        //is operator
        if (it_op1 != g_SYMapOperator.end())
        {
            auto& op1 = it_op1->second;
            auto op1_predicate = std::get<SY_Precedence>(op1);
            auto op1_assoc = std::get<SY_Associativity>(op1);

            if (stak.empty())
            {
                stak.push(token);
                continue;
            }

            while (stak.size() > 0)
            {
                auto& token2 = stak.top();
                auto it_op2 = g_SYMapOperator.find(token2);
                if (it_op2 == g_SYMapOperator.end())
                    break;

                auto& op2 = it_op2->second;
                auto op2_predicate = std::get<SY_Precedence>(op2);
                auto op2_assoc = std::get<SY_Associativity>(op2);

                auto delta = op1_predicate - op2_predicate;

                
                auto isLeft = delta < 0 || (op1_assoc == SY_Associativity::LEFT && delta <= 0);
                if (isLeft && op2_assoc != SY_Associativity::FUNCTION)
                {
                    out.push_back(token2);
                    stak.pop();
                }
                else
                {
                    break;
                }
            }

            stak.push(token);
        }
        //is (, )
        else
        {
            auto str = std::get<std::string>(token);
            if (str == ",") //ignore
            {
                continue;
            }
            if (str == "(")
            {
                stak.push(token);
            }
            else if (str == ")")
            {
                std::string top_str;
                while (stak.size() > 0) 
                {
                    auto top = stak.top();
                    top_str = std::get<std::string>(stak.top());
                    stak.pop();
                    if (top_str == "(")
                        break;
                    out.push_back(top);
                }
                if (top_str != "(") 
                    throw std::domain_error("No matching left parenthesis.");
            }
            else
            {
                throw std::domain_error("Unknow operator '" + str + "'");
            }
        }
    
    }

    while (stak.size() > 0)
    {
        out.push_back(stak.top());
        stak.pop();
    }

    return out;
}

RPN::TokenList RPN::shuntingYardAlg(const std::string & input)
{
    return shuntingYardAlg(tokenFromString(input));
}

double RPN::calculateShuntingYard(const std::string & input)
{
    return calculate(shuntingYardAlg(input));
}

void RPN::registorOperator(const std::string & str, const OperatorFn & fn)
{
    g_mapOperator.emplace(str, fn);
}

void RPN::registerSYOperator(const Token & token, const SY_Operator & op)
{
    g_SYMapOperator.emplace(token, op);
}
