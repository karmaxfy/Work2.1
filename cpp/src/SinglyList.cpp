#include "SinglyList.h"
#include <stdexcept>

SinglyList::~SinglyList() {
    while (head_ != nullptr) {
        Node* old = head_;
        head_ = head_->next;
        delete old;
    }
}

void SinglyList::push_front(const Text& value) {
    Node* node = new Node{value, head_};
    head_ = node;
    if (tail_ == nullptr) {
        tail_ = node;
    }
    ++size_;
}

void SinglyList::push_back(const Text& value) {
    Node* node = new Node{value, nullptr};
    if (tail_ != nullptr) {
        tail_->next = node;
    }
    if (head_ == nullptr) {
        head_ = node;
    }
    tail_ = node;
    ++size_;
}

bool SinglyList::after(const Text& target, const Text& value) {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value != target) {
            continue;
        }
        Node* added = new Node{value, node->next};
        node->next = added;
        if (tail_ == node) {
            tail_ = added;
        }
        ++size_;
        return true;
    }
    return false;
}

bool SinglyList::before(const Text& target, const Text& value) {
    if (head_ != nullptr && head_->value == target) {
        push_front(value);
        return true;
    }
    for (Node* node = head_; node != nullptr && node->next != nullptr; node = node->next) {
        if (node->next->value != target) {
            continue;
        }
        node->next = new Node{value, node->next};
        ++size_;
        return true;
    }
    return false;
}

Text SinglyList::pop_front() {
    if (head_ == nullptr) {
        throw std::runtime_error("список пуст");
    }
    Node* old = head_;
    Text value = old->value;
    head_ = head_->next;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    delete old;
    --size_;
    return value;
}

Text SinglyList::pop_back() {
    if (head_ == nullptr) {
        throw std::runtime_error("список пуст");
    }
    if (head_ == tail_) {
        return pop_front();
    }
    Node* node = head_;
    while (node->next != tail_) {
        node = node->next;
    }
    Text value = tail_->value;
    delete tail_;
    tail_ = node;
    tail_->next = nullptr;
    --size_;
    return value;
}

bool SinglyList::remove(const Text& value) {
    if (head_ != nullptr && head_->value == value) {
        pop_front();
        return true;
    }
    for (Node* node = head_; node != nullptr && node->next != nullptr; node = node->next) {
        if (node->next->value != value) {
            continue;
        }
        Node* old = node->next;
        node->next = old->next;
        if (tail_ == old) {
            tail_ = node;
        }
        delete old;
        --size_;
        return true;
    }
    return false;
}

// Удаление перед первым найденным значением.
bool SinglyList::remove_before(const Text& target) {
    Node* previousPrevious = nullptr;
    Node* previous = nullptr;
    Node* current = head_;
    while (current != nullptr && current->value != target) {
        previousPrevious = previous;
        previous = current;
        current = current->next;
    }
    if (current == nullptr || previous == nullptr) return false;
    if (previousPrevious == nullptr) head_ = current;
    else previousPrevious->next = current;
    delete previous;
    --size_;
    return true;
}

// Удаление после первого найденного значения.
bool SinglyList::remove_after(const Text& target) {
    for (Node* current = head_; current != nullptr; current = current->next) {
        if (current->value != target) continue;
        if (current->next == nullptr) return false;
        Node* removed = current->next;
        current->next = removed->next;
        if (tail_ == removed) tail_ = current;
        delete removed;
        --size_;
        return true;
    }
    return false;
}

Text SinglyList::front() const {
    if (head_ == nullptr) throw std::runtime_error("список пуст");
    return head_->value;
}

Text SinglyList::back() const {
    if (tail_ == nullptr) throw std::runtime_error("список пуст");
    return tail_->value;
}

Text SinglyList::get(std::size_t index) const {
    Node* node = head_;
    for (std::size_t i = 0; i < index && node != nullptr; ++i) {
        node = node->next;
    }
    if (node == nullptr) {
        throw std::out_of_range("индекс вне диапазона");
    }
    return node->value;
}

bool SinglyList::contains(const Text& value) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return true;
        }
    }
    return false;
}

std::vector<Text> SinglyList::values() const {
    std::vector<Text> result;
    for (Node* node = head_; node != nullptr; node = node->next) {
        result.push_back(node->value);
    }
    return result;
}

bool SinglyList::empty() const {
    return head_ == nullptr;
}
