#pragma once

#include "Text.h"
#include <cstddef>
#include <vector>

// Узел односвязного списка
class SinglyList {
public:
    ~SinglyList();
    SinglyList() = default;
    SinglyList(const SinglyList&) = delete;
    SinglyList& operator=(const SinglyList&) = delete;

    void push_front(const Text& value);
    void push_back(const Text& value);
    bool before(const Text& target, const Text& value);
    bool after(const Text& target, const Text& value);
    Text pop_front();
    Text pop_back();
    bool remove(const Text& value);
    bool remove_before(const Text& target);
    bool remove_after(const Text& target);
    Text front() const;
    Text back() const;
    Text get(std::size_t index) const;
    bool contains(const Text& value) const;
    std::vector<Text> values() const;
    bool empty() const;

private:
    struct Node {
        Text value;
        Node* next = nullptr;
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};
