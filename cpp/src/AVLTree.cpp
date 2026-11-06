#include "AVLTree.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <queue>
#include <stdexcept>

AVLTree::~AVLTree() {
    clear();
}

int AVLTree::height(const Node* node) {
    return node == nullptr ? 0 : node->height;
}

void AVLTree::update(Node* node) {
    node->height = 1 + std::max(height(node->left), height(node->right));
}

int AVLTree::factor(const Node* node) {
    return height(node->left) - height(node->right);
}

// Повороты изменяют связи, сохраняя порядок ключей.
AVLTree::Node* AVLTree::rotateLeft(Node* node) {
    Node* top = node->right;
    node->right = top->left;
    top->left = node;
    update(node);
    update(top);
    return top;
}

AVLTree::Node* AVLTree::rotateRight(Node* node) {
    Node* top = node->left;
    node->left = top->right;
    top->right = node;
    update(node);
    update(top);
    return top;
}

AVLTree::Node* AVLTree::balance(Node* node) {
    update(node);

    if (factor(node) > 1) {
        if (factor(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }

    if (factor(node) < -1) {
        if (factor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }

    return node;
}

AVLTree::Node* AVLTree::add(Node* node, int value, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new Node{value};
    }
    if (value == node->value) {
        return node;
    }
    if (value < node->value) {
        node->left = add(node->left, value, inserted);
    }
    if (value > node->value) {
        node->right = add(node->right, value, inserted);
    }
    return balance(node);
}

AVLTree::Node* AVLTree::minNode(Node* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

AVLTree::Node* AVLTree::remove(Node* node, int value, bool& removed) {
    if (node == nullptr) {
        return nullptr;
    }
    if (value < node->value) {
        node->left = remove(node->left, value, removed);
    }
    if (value > node->value) {
        node->right = remove(node->right, value, removed);
    }
    if (value == node->value) {
        removed = true;
        if (node->left == nullptr || node->right == nullptr) {
            Node* child = node->left == nullptr ? node->right : node->left;
            delete node;
            return child;
        }
        Node* successor = minNode(node->right);
        node->value = successor->value;
        bool ignored = false;
        node->right = remove(node->right, successor->value, ignored);
    }
    return balance(node);
}

bool AVLTree::insert(int value) {
    bool inserted = false;
    root_ = add(root_, value, inserted);
    return inserted;
}

bool AVLTree::erase(int value) {
    bool removed = false;
    root_ = remove(root_, value, removed);
    return removed;
}

bool AVLTree::contains(int value) const {
    const Node* node = root_;
    while (node != nullptr) {
        if (node->value == value) {
            return true;
        }
        node = value < node->value ? node->left : node->right;
    }
    return false;
}

void AVLTree::destroy(Node* node) {
    if (node == nullptr) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
}

void AVLTree::clear() {
    destroy(root_);
    root_ = nullptr;
}

// 0 = inorder, 1 = preorder, 2 = postorder.
void AVLTree::walk(const Node* node, int order, std::vector<int>& result) {
    if (node == nullptr) {
        return;
    }
    if (order == 1) {
        result.push_back(node->value);
    }
    walk(node->left, order, result);
    if (order == 0) {
        result.push_back(node->value);
    }
    walk(node->right, order, result);
    if (order == 2) {
        result.push_back(node->value);
    }
}

std::vector<int> AVLTree::inorder() const {
    std::vector<int> result;
    walk(root_, 0, result);
    return result;
}

std::vector<int> AVLTree::preorder() const {
    std::vector<int> result;
    walk(root_, 1, result);
    return result;
}

std::vector<int> AVLTree::postorder() const {
    std::vector<int> result;
    walk(root_, 2, result);
    return result;
}

std::vector<int> AVLTree::breadthFirst() const {
    std::vector<int> result;
    std::queue<const Node*> pending;
    if (root_ != nullptr) {
        pending.push(root_);
    }
    while (!pending.empty()) {
        const Node* node = pending.front();
        pending.pop();
        result.push_back(node->value);
        if (node->left != nullptr) {
            pending.push(node->left);
        }
        if (node->right != nullptr) {
            pending.push(node->right);
        }
    }
    return result;
}

void AVLTree::draw(const Node* node, int depth, std::string& result) {
    if (node == nullptr) {
        return;
    }
    draw(node->right, depth + 1, result);
    result += std::string(depth * 4, ' ') + std::to_string(node->value);
    result += " (h=" + std::to_string(node->height) + ")\n";
    draw(node->left, depth + 1, result);
}

std::string AVLTree::print() const {
    std::string result;
    draw(root_, 0, result);
    return result.empty() ? "(empty)" : result;
}

int AVLTree::minimum() const {
    if (root_ == nullptr) {
        throw std::runtime_error("дерево пусто");
    }
    return minNode(root_)->value;
}

int AVLTree::maximum() const {
    if (root_ == nullptr) {
        throw std::runtime_error("дерево пусто");
    }
    const Node* node = root_;
    while (node->right != nullptr) {
        node = node->right;
    }
    return node->value;
}

int AVLTree::next(int value) const {
    const Node* node = root_;
    const Node* candidate = nullptr;
    while (node != nullptr) {
        if (node->value > value) {
            candidate = node;
            node = node->left;
            continue;
        }
        node = node->right;
    }
    if (candidate == nullptr) {
        throw std::runtime_error("следующего ключа нет");
    }
    return candidate->value;
}

int AVLTree::previous(int value) const {
    const Node* node = root_;
    const Node* candidate = nullptr;
    while (node != nullptr) {
        if (node->value < value) {
            candidate = node;
            node = node->right;
            continue;
        }
        node = node->left;
    }
    if (candidate == nullptr) {
        throw std::runtime_error("предыдущего ключа нет");
    }
    return candidate->value;
}

// Возвращает высоту; -1 означает нарушение порядка или баланса.
int AVLTree::check(const Node* node, long long low, long long high) {
    if (node == nullptr) {
        return 0;
    }
    if (node->value <= low || node->value >= high) {
        return -1;
    }
    int left = check(node->left, low, node->value);
    int right = check(node->right, node->value, high);
    if (left < 0 || right < 0) {
        return -1;
    }
    if (std::abs(left - right) > 1) {
        return -1;
    }
    if (node->height != 1 + std::max(left, right)) {
        return -1;
    }
    return node->height;
}

bool AVLTree::valid() const {
    return check(root_, LLONG_MIN, LLONG_MAX) >= 0;
}
