#pragma once

#include "SinglyList.h"
#include "Text.h"
#include <vector>



class Stack {
public:
    void push(const Text& value);
    Text pop();
    Text top() const;
    std::vector<Text> values() const;
private:
    SinglyList list_;
};
