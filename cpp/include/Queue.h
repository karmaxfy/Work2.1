#pragma once

#include "SinglyList.h"
#include "Text.h"
#include <vector>

// Очередь FIFO.
class Queue {
public:
    void push(const Text& value);
    Text pop();
    Text front() const;
    std::vector<Text> values() const;
private:
    SinglyList list_;
};
