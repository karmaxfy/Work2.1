#include "DoublyList.h"
#include <stdexcept>

DoublyList::~DoublyList() {
    while (head_ != nullptr) {
        Node* old = head_;
        head_ = head_->next;
        delete old;
    }
}

DoublyList::Node* DoublyList::find(const Text& value) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return node;
        }
    }
    return nullptr;
}

void DoublyList::push_front(const Text& value) {
    Node* node = new Node{value, nullptr, head_};
    if (head_ != nullptr) {
        head_->prev = node;
    }
    if (tail_ == nullptr) {
        tail_ = node;
    }
    head_ = node;
}

void DoublyList::push_back(const Text& value) {
    Node* node = new Node{value, tail_, nullptr};
    if (tail_ != nullptr) {
        tail_->next = node;
    }
    if (head_ == nullptr) {
        head_ = node;
    }
    tail_ = node;
}

bool DoublyList::after(const Text& target, const Text& value) {
    Node* node = find(target);
    if (node == nullptr) {
        return false;
    }
    if (node == tail_) {
        push_back(value);
        return true;
    }
    Node* added = new Node{value, node, node->next};
    node->next->prev = added;
    node->next = added;
    return true;
}

bool DoublyList::before(const Text& target, const Text& value) {
    Node* node = find(target);
    if (node == nullptr) {
        return false;
    }
    if (node == head_) {
        push_front(value);
        return true;
    }
    Node* added = new Node{value, node->prev, node};
    node->prev->next = added;
    node->prev = added;
    return true;
}

Text DoublyList::unlink(Node* node) {
    Text value = node->value;
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    }
    if (head_ == node) {
        head_ = node->next;
    }
    if (tail_ == node) {
        tail_ = node->prev;
    }
    delete node;
    return value;
}

Text DoublyList::pop_front() {
    if (head_ == nullptr) {
        throw std::runtime_error("список пуст");
    }
    return unlink(head_);
}

Text DoublyList::pop_back() {
    if (tail_ == nullptr) {
        throw std::runtime_error("список пуст");
    }
    return unlink(tail_);
}

bool DoublyList::remove(const Text& value) {
    Node* node = find(value);
    if (node == nullptr) {
        return false;
    }
    unlink(node);
    return true;
}

Text DoublyList::get(std::size_t index) const {
    Node* node = head_;
    for (std::size_t i = 0; i < index && node != nullptr; ++i) {
        node = node->next;
    }
    if (node == nullptr) {
        throw std::out_of_range("индекс вне диапазона");
    }
    return node->value;
}

bool DoublyList::contains(const Text& value) const {
    return find(value) != nullptr;
}

std::vector<Text> DoublyList::values() const {
    std::vector<Text> result;
    for (Node* node = head_; node != nullptr; node = node->next) {
        result.push_back(node->value);
    }
    return result;
}

std::vector<Text> DoublyList::reverse() const {
    std::vector<Text> result;
    for (Node* node = tail_; node != nullptr; node = node->prev) {
        result.push_back(node->value);
    }
    return result;
}
