#include "Database.h"

#include <iostream>
#include <map>
#include <string>

// Выбор раздела без цепочки if/else.
char menu(char choice) {
    switch (choice) {
        case '1': return 'M';
        case '2': return 'F';
        case '3': return 'L';
        case '4': return 'S';
        case '5': return 'Q';
        case '6': return 'T';
        default: return 0;
    }
}

int main(int argc, char* argv[]) {
    std::string file = "file.data";
    std::string query;
    bool single = false;

    // Номера параметров позволяют использовать switch вместо if/else.
    const std::map<std::string, int> options = {
        {"--file", 1}, {"--query", 2}
    };
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (i + 1 >= argc) {
            std::cerr << "Использование: ./1laba --file file.data --query 'COMMAND'\n";
            return 1;
        }
        auto option = options.find(arg);
        int action = option == options.end() ? 0 : option->second;
        switch (action) {
            case 1: file = argv[++i]; break;
            case 2: query = argv[++i]; single = true; break;
            default:
                std::cerr << "Неизвестный параметр: " << arg << '\n';
                return 1;
        }
    }

    try {
        Database db(file);
        if (single) {
            const std::string answer = db.execute(query);
            if (!answer.empty()) {
                std::cout << answer << '\n';
            }
            return answer.rfind("Ошибка:", 0) == 0 ? 1 : 0;
        }

        std::cout << "1 Массив | 2 Односвязный | 3 Двусвязный | 4 Стек | 5 Очередь | 6 АВЛ | 0 Выход\n";
        std::cout << "Команды можно вводить напрямую. HELP - справка.\n";

        char section = 0;
        std::string line;
        while (true) {
            std::cout << (section == 0 ? "Choice> " : "> ");
            if (!std::getline(std::cin, line) || line == "0") {
                break;
            }

            if (line.size() == 1 && section == 0 && menu(line[0]) != 0) {
                section = menu(line[0]);
                std::cout << db.summary(section) << '\n';
                continue;
            }

            const std::string answer = db.execute(line);
            if (answer == "EXIT") {
                break;
            }
            if (answer == "BACK") {
                section = 0;
                continue;
            }
            if (!answer.empty()) {
                std::cout << answer << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
