#include "Database.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {

using Words = std::vector<std::string>;

enum class Op {
    Help, Back, Exit, Print, Create,
    Push, PushFront, Insert, Get, Set, Delete, Length,
    Before, After, DeleteHead, DeleteTail, DeleteBefore, DeleteAfter, ReadHead, ReadTail, Find, Reverse,
    Pop, TreeInsert, TreeDelete, TreeGet, Inorder,
    Preorder, Postorder, BFS, Min, Max, Next, Prev, Check
};

struct Command {
    Op op;
    char kind;
    std::size_t count;
};

// Имя команды, операция, тип контейнера и число слов.
const std::unordered_map<std::string, Command> commands = {
    {"HELP", {Op::Help, ' ', 1}}, {"BACK", {Op::Back, ' ', 1}},
    {"EXIT", {Op::Exit, ' ', 1}}, {"PRINT", {Op::Print, ' ', 3}},
    {"MCREATE", {Op::Create, 'M', 2}},
    {"MPUSH", {Op::Push, 'M', 3}}, {"MINSERT", {Op::Insert, 'M', 4}},
    {"MGET", {Op::Get, 'M', 3}}, {"MSET", {Op::Set, 'M', 4}},
    {"MDEL", {Op::Delete, 'M', 3}}, {"MLEN", {Op::Length, 'M', 2}},
    {"FCREATE", {Op::Create, 'F', 2}},
    {"FPUSH", {Op::Push, 'F', 3}}, {"FPUSHFRONT", {Op::PushFront, 'F', 3}},
    {"FAFTER", {Op::After, 'F', 4}}, {"FBEFORE", {Op::Before, 'F', 4}},
    {"FDEL", {Op::Delete, 'F', 3}}, {"FDELHEAD", {Op::DeleteHead, 'F', 2}},
    {"FDELTAIL", {Op::DeleteTail, 'F', 2}}, {"FGET", {Op::Get, 'F', 3}},
    {"FFIND", {Op::Find, 'F', 3}},
    {"FDELBEFORE", {Op::DeleteBefore, 'F', 3}},
    {"FDELAFTER", {Op::DeleteAfter, 'F', 3}},
    {"FGETHEAD", {Op::ReadHead, 'F', 2}},
    {"FGETTAIL", {Op::ReadTail, 'F', 2}},
    {"LCREATE", {Op::Create, 'L', 2}},
    {"LPUSH", {Op::Push, 'L', 3}}, {"LPUSHFRONT", {Op::PushFront, 'L', 3}},
    {"LAFTER", {Op::After, 'L', 4}}, {"LBEFORE", {Op::Before, 'L', 4}},
    {"LDEL", {Op::Delete, 'L', 3}}, {"LDELHEAD", {Op::DeleteHead, 'L', 2}},
    {"LDELTAIL", {Op::DeleteTail, 'L', 2}}, {"LGET", {Op::Get, 'L', 3}},
    {"LFIND", {Op::Find, 'L', 3}}, {"LREVERSE", {Op::Reverse, 'L', 2}},
    {"LDELBEFORE", {Op::DeleteBefore, 'L', 3}},
    {"LDELAFTER", {Op::DeleteAfter, 'L', 3}},
    {"LGETHEAD", {Op::ReadHead, 'L', 2}},
    {"LGETTAIL", {Op::ReadTail, 'L', 2}},
    {"SCREATE", {Op::Create, 'S', 2}},
    {"SPUSH", {Op::Push, 'S', 3}}, {"SPOP", {Op::Pop, 'S', 2}},
    {"SGET", {Op::Get, 'S', 2}},
    {"QCREATE", {Op::Create, 'Q', 2}},
    {"QPUSH", {Op::Push, 'Q', 3}}, {"QPOP", {Op::Pop, 'Q', 2}},
    {"QGET", {Op::Get, 'Q', 2}},
    {"TCREATE", {Op::Create, 'T', 2}},
    {"TINSERT", {Op::TreeInsert, 'T', 3}},
    {"TDEL", {Op::TreeDelete, 'T', 3}},
    {"TGET", {Op::TreeGet, 'T', 3}},
    {"ISMEMBER", {Op::TreeGet, 'T', 3}},
    {"TINORDER", {Op::Inorder, 'T', 2}},
    {"TPREORDER", {Op::Preorder, 'T', 2}},
    {"TPOSTORDER", {Op::Postorder, 'T', 2}},
    {"TBFS", {Op::BFS, 'T', 2}},
    {"TMIN", {Op::Min, 'T', 2}}, {"TMAX", {Op::Max, 'T', 2}},
    {"TNEXT", {Op::Next, 'T', 3}}, {"TPREV", {Op::Prev, 'T', 3}},
    {"TCHECK", {Op::Check, 'T', 2}}
};

Words splitWords(const std::string& text) {
    std::istringstream input(text);
    Words words;
    std::string word;
    while (input >> word) {
        words.push_back(word);
    }
    return words;
}

std::string upper(std::string text) {
    for (char& c : text) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return text;
}

int number(const std::string& text) {
    std::size_t used = 0;
    int value = std::stoi(text, &used);
    if (used != text.size()) {
        throw std::runtime_error("ожидается целое число");
    }
    return value;
}

std::size_t indexOf(const std::string& text) {
    int value = number(text);
    if (value < 0) {
        throw std::runtime_error("индекс не может быть отрицательным");
    }
    return static_cast<std::size_t>(value);
}

std::string asText(bool value) {
    return value ? "TRUE" : "FALSE";
}

std::string format(const std::vector<std::string>& values) {
    std::string text = "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            text += ", ";
        }
        text += values[i];
    }
    return text + "]";
}

std::string formatInts(const std::vector<int>& values) {
    std::vector<std::string> strings;
    for (int value : values) {
        strings.push_back(std::to_string(value));
    }
    return format(strings);
}

// Табуляция разделяет поля; специальные символы экранируются.
std::string escape(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\t': result += "\\t"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            default: result += c;
        }
    }
    return result;
}

std::string unescape(const std::string& text) {
    std::string result;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] != '\\') {
            result += text[i];
            continue;
        }
        if (++i == text.size()) {
            throw std::runtime_error("неправильное экранирование в файле");
        }
        switch (text[i]) {
            case 't': result += '\t'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case '\\': result += '\\'; break;
            default: throw std::runtime_error("неправильное экранирование в файле");
        }
    }
    return result;
}

Words splitFields(const std::string& text) {
    Words result;
    std::size_t start = 0;
    for (std::size_t i = 0; i <= text.size(); ++i) {
        if (i != text.size() && text[i] != '\t') {
            continue;
        }
        result.push_back(unescape(text.substr(start, i - start)));
        start = i + 1;
    }
    return result;
}

template <typename Map>
typename Map::mapped_type& require(Map& items, const std::string& name) {
    auto it = items.find(name);
    if (it == items.end()) {
        throw std::runtime_error("контейнер не найден");
    }
    return it->second;
}

template <typename Map>
const typename Map::mapped_type& require(const Map& items, const std::string& name) {
    auto it = items.find(name);
    if (it == items.end()) {
        throw std::runtime_error("контейнер не найден");
    }
    return it->second;
}

// Одинаковые действия для двух видов списков.
template <typename List>
std::string listAction(List& list, Op op, const Words& args, bool& changed) {
    switch (op) {
        case Op::Push: list.push_back(args[2]); changed = true; return "OK";
        case Op::PushFront: list.push_front(args[2]); changed = true; return "OK";
        case Op::After:
            if (!list.after(args[2], args[3])) return "NOT FOUND";
            changed = true;
            return "OK";
        case Op::Before:
            if (!list.before(args[2], args[3])) return "NOT FOUND";
            changed = true;
            return "OK";
        case Op::Delete:
            if (!list.remove(args[2])) return "NOT FOUND";
            changed = true;
            return "OK";
        case Op::DeleteBefore:
            if (!list.remove_before(args[2])) return "NOT FOUND";
            changed = true;
            return "OK";
        case Op::DeleteAfter:
            if (!list.remove_after(args[2])) return "NOT FOUND";
            changed = true;
            return "OK";
        case Op::ReadHead: return list.front();
        case Op::ReadTail: return list.back();
        case Op::DeleteHead: changed = true; return list.pop_front();
        case Op::DeleteTail: changed = true; return list.pop_back();
        case Op::Get: return list.get(indexOf(args[2]));
        case Op::Find: return asText(list.contains(args[2]));
        default: throw std::runtime_error("неизвестная операция списка");
    }
}

const std::string help =
    "M: MCREATE MPUSH MINSERT MGET MSET MDEL MLEN\n"
    "F/L: FCREATE LCREATE FPUSH LPUSH FPUSHFRONT LPUSHFRONT FAFTER LAFTER "
    "FBEFORE LBEFORE FDEL LDEL FDELHEAD LDELHEAD FDELTAIL LDELTAIL "
    "FGET LGET FFIND LFIND LREVERSE FDELBEFORE FDELAFTER "
    "LDELBEFORE LDELAFTER FGETHEAD FGETTAIL LGETHEAD LGETTAIL\n"
    "S: SCREATE SPUSH SPOP SGET\n"
    "Q: QCREATE QPUSH QPOP QGET\n"
    "T: TCREATE TINSERT TDEL TGET TINORDER TPREORDER TPOSTORDER "
    "TBFS TMIN TMAX TNEXT TPREV TCHECK ISMEMBER\n"
    "PRINT M|F|L|S|Q|T name; BACK; EXIT";

} // namespace

Database::Database(std::filesystem::path file) : file_(std::move(file)) {
    load();
}

Database::Items Database::snapshot(char kind) const {
    Items result;
    switch (kind) {
        case 'M':
            for (const auto& item : arrays_) result[item.first] = item.second.values();
            break;
        case 'F':
            for (const auto& item : singles_) result[item.first] = item.second.values();
            break;
        case 'L':
            for (const auto& item : doubles_) result[item.first] = item.second.values();
            break;
        case 'S':
            for (const auto& item : stacks_) {
                auto values = item.second.values();
                std::reverse(values.begin(), values.end());
                result[item.first] = std::move(values);
            }
            break;
        case 'Q':
            for (const auto& item : queues_) result[item.first] = item.second.values();
            break;
        case 'T':
            for (const auto& item : trees_) {
                std::vector<std::string> values;
                for (int value : item.second.inorder()) values.push_back(std::to_string(value));
                result[item.first] = std::move(values);
            }
            break;
    }
    return result;
}

void Database::save() const {
    if (file_.has_parent_path()) {
        std::filesystem::create_directories(file_.parent_path());
    }
    auto temporary = file_;
    temporary += ".tmp";
    std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("невозможно открыть файл для записи");
    }

    output << "# LAB2-1\n";
    for (char kind : std::string("MFLSQT")) {
        for (const auto& item : snapshot(kind)) {
            output << kind << '\t' << escape(item.first);
            for (const std::string& value : item.second) {
                output << '\t' << escape(value);
            }
            output << '\n';
        }
    }
    output.close();
    if (!output) {
        throw std::runtime_error("ошибка записи файла");
    }
    std::filesystem::rename(temporary, file_);
}

void Database::load() {
    if (!std::filesystem::exists(file_)) {
        return;
    }
    std::ifstream input(file_, std::ios::binary);
    if (!input) {
        throw std::runtime_error("невозможно открыть файл для чтения");
    }
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const Words fields = splitFields(line);
        if (fields.size() < 2 || fields[0].size() != 1 || fields[1].empty()) {
            throw std::runtime_error("неправильный формат файла");
        }
        const char kind = fields[0][0];
        const std::string& name = fields[1];
        if (name.find(':') != std::string::npos) {
            throw std::runtime_error("недопустимое имя контейнера");
        }
        switch (kind) {
            case 'M': {
                Array& container = arrays_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) container.push(fields[i]);
                break;
            }
            case 'F': {
                SinglyList& container = singles_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) container.push_back(fields[i]);
                break;
            }
            case 'L': {
                DoublyList& container = doubles_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) container.push_back(fields[i]);
                break;
            }
            case 'S': {
                Stack& container = stacks_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) container.push(fields[i]);
                break;
            }
            case 'Q': {
                Queue& container = queues_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) container.push(fields[i]);
                break;
            }
            case 'T': {
                AVLTree& container = trees_.try_emplace(name).first->second;
                for (std::size_t i = 2; i < fields.size(); ++i) {
                    if (!container.insert(number(fields[i]))) throw std::runtime_error("повтор ключа в дереве");
                }
                break;
            }
            default: throw std::runtime_error("неизвестный тип данных в файле");
        }
    }
}

std::string Database::print(char kind, const std::string& name) const {
    switch (kind) {
        case 'M': return format(require(arrays_, name).values());
        case 'F': return format(require(singles_, name).values());
        case 'L': return format(require(doubles_, name).values());
        case 'S': return format(require(stacks_, name).values());
        case 'Q': return format(require(queues_, name).values());
        case 'T': return require(trees_, name).print();
        default: throw std::runtime_error("неизвестный тип структуры");
    }
}

std::string Database::summary(char kind) const {
    std::string output;
    for (const auto& item : snapshot(kind)) {
        output += item.first + ": " + format(item.second) + "\n";
    }
    return output.empty() ? "(none)" : output;
}

std::string Database::execute(const std::string& line) {
    try {
        Words args = splitWords(line);
        if (args.empty()) {
            return "";
        }
        args[0] = upper(args[0]);
        auto command = commands.find(args[0]);
        if (command == commands.end()) {
            throw std::runtime_error("неизвестная команда (HELP - справка)");
        }
        const Command spec = command->second;
        if (args.size() != spec.count) {
            throw std::runtime_error("неверное число аргументов");
        }

        if (spec.op == Op::Help) return help;
        if (spec.op == Op::Back) return "BACK";
        if (spec.op == Op::Exit) return "EXIT";
        if (spec.op == Op::Print) {
            if (args[1].size() != 1) throw std::runtime_error("неизвестный тип структуры");
            return print(upper(args[1])[0], args[2]);
        }

        const std::string& name = args[1];
        if (name.empty() || name.find(':') != std::string::npos) {
            throw std::runtime_error("неверное имя контейнера");
        }

        if (spec.op == Op::Create) {
            bool created = false;
            switch (spec.kind) {
                case 'M': created = arrays_.try_emplace(name).second; break;
                case 'F': created = singles_.try_emplace(name).second; break;
                case 'L': created = doubles_.try_emplace(name).second; break;
                case 'S': created = stacks_.try_emplace(name).second; break;
                case 'Q': created = queues_.try_emplace(name).second; break;
                case 'T': created = trees_.try_emplace(name).second; break;
            }
            if (created) save();
            return created ? "OK" : "EXISTS";
        }

        bool changed = false;
        std::string result = "OK";

        switch (spec.kind) {
            case 'M': {
                Array& a = spec.op == Op::Push
                    ? arrays_.try_emplace(name).first->second
                    : require(arrays_, name);
                switch (spec.op) {
                    case Op::Push: a.push(args[2]); changed = true; break;
                    case Op::Insert: a.insert(indexOf(args[2]), args[3]); changed = true; break;
                    case Op::Get: result = a.get(indexOf(args[2])); break;
                    case Op::Set: a.set(indexOf(args[2]), args[3]); changed = true; break;
                    case Op::Delete: result = a.erase(indexOf(args[2])); changed = true; break;
                    case Op::Length: result = std::to_string(a.size()); break;
                    default: break;
                }
                break;
            }
            case 'F': {
                SinglyList& a = spec.op == Op::Push || spec.op == Op::PushFront
                    ? singles_.try_emplace(name).first->second
                    : require(singles_, name);
                result = listAction(a, spec.op, args, changed);
                break;
            }
            case 'L': {
                DoublyList& a = spec.op == Op::Push || spec.op == Op::PushFront
                    ? doubles_.try_emplace(name).first->second
                    : require(doubles_, name);
                result = spec.op == Op::Reverse
                    ? format(a.reverse()) : listAction(a, spec.op, args, changed);
                break;
            }
            case 'S': {
                Stack& s = spec.op == Op::Push
                    ? stacks_.try_emplace(name).first->second
                    : require(stacks_, name);
                switch (spec.op) {
                    case Op::Push: s.push(args[2]); changed = true; break;
                    case Op::Pop: result = s.pop(); changed = true; break;
                    case Op::Get: result = s.top(); break;
                    default: break;
                }
                break;
            }
            case 'Q': {
                Queue& q = spec.op == Op::Push
                    ? queues_.try_emplace(name).first->second
                    : require(queues_, name);
                switch (spec.op) {
                    case Op::Push: q.push(args[2]); changed = true; break;
                    case Op::Pop: result = q.pop(); changed = true; break;
                    case Op::Get: result = q.front(); break;
                    default: break;
                }
                break;
            }
            case 'T': {
                AVLTree& t = spec.op == Op::TreeInsert
                    ? trees_.try_emplace(name).first->second
                    : require(trees_, name);
                switch (spec.op) {
                    case Op::TreeInsert:
                        if (!t.insert(number(args[2]))) return "EXISTS";
                        changed = true;
                        break;
                    case Op::TreeDelete:
                        if (!t.erase(number(args[2]))) return "NOT FOUND";
                        changed = true;
                        break;
                    case Op::TreeGet: result = asText(t.contains(number(args[2]))); break;
                    case Op::Inorder: result = formatInts(t.inorder()); break;
                    case Op::Preorder: result = formatInts(t.preorder()); break;
                    case Op::Postorder: result = formatInts(t.postorder()); break;
                    case Op::BFS: result = formatInts(t.breadthFirst()); break;
                    case Op::Min: result = std::to_string(t.minimum()); break;
                    case Op::Max: result = std::to_string(t.maximum()); break;
                    case Op::Next: result = std::to_string(t.next(number(args[2]))); break;
                    case Op::Prev: result = std::to_string(t.previous(number(args[2]))); break;
                    case Op::Check: result = asText(t.valid()); break;
                    default: break;
                }
                break;
            }
        }

        if (changed) {
            save();
        }
        return result;
    } catch (const std::exception& error) {
        return std::string("Ошибка: ") + error.what();
    }
}
