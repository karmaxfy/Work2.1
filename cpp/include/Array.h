#pragma once

#include "Text.h"
#include <cstddef>
#include <memory>
#include <vector>

// Массив с динамическим буфером.
class Array {
public:
    void push(const Text& value);
    void insert(std::size_t index, const Text& value);
    Text erase(std::size_t index);
    Text get(std::size_t index) const;
    void set(std::size_t index, const Text& value);
    std::size_t size() const;
    std::vector<Text> values() const;

private:
    void grow();
    std::unique_ptr<Text[]> data_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
