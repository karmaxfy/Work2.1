#pragma once

#include <string>
#include <vector>

class AVLTree {
public:
    AVLTree() = default;
    ~AVLTree();
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool insert(int value);
    bool erase(int value);
    bool contains(int value) const;
    bool valid() const;
    void clear();

    std::vector<int> inorder() const;
    std::vector<int> preorder() const;
    std::vector<int> postorder() const;
    std::vector<int> breadthFirst() const;
    std::string print() const;
    int minimum() const;
    int maximum() const;
    int next(int value) const;
    int previous(int value) const;

private:
    struct Node {
        int value;
        int height = 1;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    Node* root_ = nullptr;

    static int height(const Node* node);
    static void update(Node* node);
    static int factor(const Node* node);
    static Node* rotateLeft(Node* node);
    static Node* rotateRight(Node* node);
    static Node* balance(Node* node);
    static Node* add(Node* node, int value, bool& inserted);
    static Node* remove(Node* node, int value, bool& removed);
    static Node* minNode(Node* node);
    static void destroy(Node* node);
    static void walk(const Node* node, int order, std::vector<int>& result);
    static void draw(const Node* node, int depth, std::string& result);
    static int check(const Node* node, long long low, long long high);
};
