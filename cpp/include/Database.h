#pragma once

#include "AVLTree.h"
#include "Array.h"
#include "SinglyList.h"
#include "DoublyList.h"
#include "Stack.h"
#include "Queue.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

class Database {
public:
    explicit Database(std::filesystem::path file);
    std::string execute(const std::string& line);
    std::string summary(char kind) const;

private:
    using Items = std::map<std::string, std::vector<std::string>>;

    std::filesystem::path file_;
    std::map<std::string, Array> arrays_;
    std::map<std::string, SinglyList> singles_;
    std::map<std::string, DoublyList> doubles_;
    std::map<std::string, Stack> stacks_;
    std::map<std::string, Queue> queues_;
    std::map<std::string, AVLTree> trees_;

    void load();
    void save() const;
    Items snapshot(char kind) const;
    std::string print(char kind, const std::string& name) const;
};
