
#include "../include/ExpressionEvaluator.h"
#include "../include/Stack.h"

#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <iomanip>

vector<string> ExpressionEvaluator::tokenize(
    const string& expression) {

    vector<string> tokens;
    size_t i = 0;

    while (i < expression.size()) {

        if (isspace(static_cast<unsigned char>(expression[i]))) {
            i++;
            continue;
        }

        if (isdigit(static_cast<unsigned char>(expression[i])) ||
            expression[i] == '.') {

            string number;
            int dots = 0;

            while (i < expression.size() &&
                  (isdigit(static_cast<unsigned char>(expression[i])) ||
                   expression[i] == '.')) {

                if (expression[i] == '.') dots++;

                if (dots > 1) {
                    throw runtime_error("Invalid number!");
                }

                number += expression[i];
                i++;
            }

            if (number == ".") {
                throw runtime_error("Invalid number!");
            }

            tokens.push_back(number);
        }
        else if (string("+-*/()").find(expression[i])
                 != string::npos) {

            tokens.push_back(string(1, expression[i]));
            i++;
        }
        else {
            throw runtime_error("Invalid character in expression!");
        }
    }

    return tokens;
}

bool ExpressionEvaluator::isOperator(const string& token) {
    return token == "+" || token == "-" ||
           token == "*" || token == "/" ||
           token == "u+" || token == "u-";
}

int ExpressionEvaluator::precedence(const string& op) {

    if (op == "u+" || op == "u-") return 3;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;

    return 0;
}

string ExpressionEvaluator::infixToPostfix(
    const string& expression) {

    vector<string> tokens = tokenize(expression);
    Stack<string> operators;

    stringstream output;
    bool expectOperand = true;

    for (const string& token : tokens) {

        if (isdigit(static_cast<unsigned char>(token[0])) ||
            token[0] == '.') {

            if (!expectOperand) {
                throw runtime_error("Missing operator!");
            }

            output << token << " ";
            expectOperand = false;
        }

        else if (token == "(") {

            if (!expectOperand) {
                throw runtime_error("Missing operator before '('!");
            }

            operators.push(token);
        }

        else if (token == ")") {

            if (expectOperand) {
                throw runtime_error("Invalid expression near ')'");
            }

            while (!operators.isEmpty() &&
                   operators.top() != "(") {

                output << operators.top() << " ";
                operators.pop();
            }

            if (operators.isEmpty()) {
                throw runtime_error("Mismatched parentheses!");
            }

            operators.pop();

            if (!operators.isEmpty() &&
                (operators.top() == "u+" ||
                 operators.top() == "u-")) {

                output << operators.top() << " ";
                operators.pop();
            }

            expectOperand = false;
        }

        else if (isOperator(token)) {

            string op = token;

            if (expectOperand) {

                if (op != "+" && op != "-") {
                    throw runtime_error("Invalid operator position!");
                }

                op = (op == "+") ? "u+" : "u-";
                operators.push(op);
            }
            else {

                while (!operators.isEmpty() &&
                       operators.top() != "(" &&
                       precedence(operators.top()) >= precedence(op)) {

                    output << operators.top() << " ";
                    operators.pop();
                }

                operators.push(op);
                expectOperand = true;
            }
        }
    }

    if (expectOperand && !tokens.empty()) {
        throw runtime_error("Incomplete expression!");
    }

    while (!operators.isEmpty()) {

        if (operators.top() == "(") {
            throw runtime_error("Mismatched parentheses!");
        }

        output << operators.top() << " ";
        operators.pop();
    }

    if (tokens.empty()) {
        throw runtime_error("Expression cannot be empty!");
    }

    return output.str();
}

double ExpressionEvaluator::evaluatePostfix(
    const string& postfix) {

    Stack<double> values;
    stringstream ss(postfix);
    string token;

    while (ss >> token) {

        if (token == "u+" || token == "u-") {

            if (values.isEmpty()) {
                throw runtime_error("Invalid postfix expression!");
            }

            double a = values.top();
            values.pop();

            values.push(token == "u-" ? -a : a);
        }

        else if (isOperator(token)) {

            if (values.size() < 2) {
                throw runtime_error("Invalid postfix expression!");
            }

            double b = values.top();
            values.pop();

            double a = values.top();
            values.pop();

            double result = 0;

            if (token == "+") result = a + b;
            else if (token == "-") result = a - b;
            else if (token == "*") result = a * b;
            else if (token == "/") {

                if (b == 0) {
                    throw runtime_error("Division by zero!");
                }

                result = a / b;
            }

            values.push(result);
        }

        else {
            try {
                size_t pos;
                double number = stod(token, &pos);

                if (pos != token.size()) {
                    throw runtime_error("Invalid number!");
                }

                values.push(number);
            }
            catch (const invalid_argument&) {
                throw runtime_error("Invalid postfix token!");
            }
            catch (const out_of_range&) {
                throw runtime_error("Number out of range!");
            }
        }
    }

    if (values.size() != 1) {
        throw runtime_error("Invalid expression!");
    }

    return values.top();
}