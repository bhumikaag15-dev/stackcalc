
#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

template <typename T>
class Stack {
private:
    vector<T> data;

public:
    void push(const T& value) {
        data.push_back(value);
    }

    void pop() {
        if (isEmpty()) {
            throw runtime_error("Stack is empty!");
        }
        data.pop_back();
    }

    T top() const {
        if (isEmpty()) {
            throw runtime_error("Stack is empty!");
        }
        return data.back();
    }

    bool isEmpty() const {
        return data.empty();
    }

    int size() const {
        return static_cast<int>(data.size());
    }
};

#endif