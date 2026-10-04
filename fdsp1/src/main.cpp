
#include <iostream>
#include <iomanip>
#include <string>
#include "../include/ExpressionEvaluator.h"

using namespace std;

int main() {

    ExpressionEvaluator evaluator;
    string expression;

    // Read the expression sent by the Python server
    getline(cin, expression);

    try {

        // Convert infix expression to postfix
        string postfix = evaluator.infixToPostfix(expression);

        // Evaluate postfix expression using stack
        double result = evaluator.evaluatePostfix(postfix);

        // Print exactly two lines for Flask
        cout << postfix << endl;
        cout << setprecision(12) << result << endl;

        return 0;
    }

    catch (const exception& e) {

        cerr << e.what() << endl;
        return 1;
    }

}