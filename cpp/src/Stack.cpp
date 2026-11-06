#include "Stack.h"

void Stack::push(const Text& value) {
    list_.push_front(value);
}
Text Stack::pop() {
    return list_.pop_front();
}
Text Stack::top() const {
    return list_.get(0);
}
std::vector<Text> Stack::values() const {
    return list_.values();
}
