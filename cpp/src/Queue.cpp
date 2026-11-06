#include "Queue.h"

void Queue::push(const Text& value) {
    list_.push_back(value);
}
Text Queue::pop() {
    return list_.pop_front();
}
Text Queue::front() const {
    return list_.get(0);
}
std::vector<Text> Queue::values() const {
    return list_.values();
}
