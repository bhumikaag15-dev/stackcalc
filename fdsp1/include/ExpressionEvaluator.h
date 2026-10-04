
#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include <string>
#include <vector>
using namespace std;

class ExpressionEvaluator {
private:
    vector<string> tokenize(const string& expression);

    int precedence(const string& op);

    bool isOperator(const string& token);

public:
    string infixToPostfix(const string& expression);

    double evaluatePostfix(const string& postfix);
};

#endif