#pragma once

#include "Text.h"
#include <cstddef>
#include <vector>

// Узел двусвязного списка.
class DoublyList {
public:
    ~DoublyList();
    DoublyList() = default;
    DoublyList(const DoublyList&) = delete;
    DoublyList& operator=(const DoublyList&) = delete;

    void push_front(const Text& value);
    void push_back(const Text& value);
    bool before(const Text& target, const Text& value);
    bool after(const Text& target, const Text& value);
    Text pop_front();
    Text pop_back();
    bool remove(const Text& value);
    Text get(std::size_t index) const;
    bool contains(const Text& value) const;
    std::vector<Text> values() const;
    std::vector<Text> reverse() const;

private:
    struct Node {
        Text value;
        Node* prev = nullptr;
        Node* next = nullptr;
    };

    Node* find(const Text& value) const;
    Text unlink(Node* node);
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
};
