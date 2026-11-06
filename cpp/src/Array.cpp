#include "Array.h"
#include <stdexcept>
#include <utility>

void Array::grow() {
    const std::size_t next = capacity_ == 0 ? 4 : capacity_ * 2;
    auto buffer = std::make_unique<Text[]>(next);

    for (std::size_t i = 0; i < size_; ++i) {
        buffer[i] = std::move(data_[i]);
    }

    data_ = std::move(buffer);
    capacity_ = next;
}

void Array::push(const Text& value) {
    insert(size_, value);
}

void Array::insert(std::size_t index, const Text& value) {
    if (index > size_) {
        throw std::out_of_range("индекс вне диапазона");
    }
    if (size_ == capacity_) {
        grow();
    }
    for (std::size_t i = size_; i > index; --i) {
        data_[i] = std::move(data_[i - 1]);
    }
    data_[index] = value;
    ++size_;
}

Text Array::erase(std::size_t index) {
    Text removed = get(index);
    for (std::size_t i = index; i + 1 < size_; ++i) {
        data_[i] = std::move(data_[i + 1]);
    }
    data_[size_ - 1].clear();
    --size_;
    return removed;
}

Text Array::get(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("индекс вне диапазона");
    }
    return data_[index];
}

void Array::set(std::size_t index, const Text& value) {
    if (index >= size_) {
        throw std::out_of_range("индекс вне диапазона");
    }
    data_[index] = value;
}

std::size_t Array::size() const {
    return size_;
}

std::vector<Text> Array::values() const {
    std::vector<Text> result;
    for (std::size_t i = 0; i < size_; ++i) {
        result.push_back(data_[i]);
    }
    return result;
}
