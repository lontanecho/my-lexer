#include<string>
#include<unordered_map>
#include"data_structure/token.h"

class TranslateTable {
public:
    TranslateTable()=default;
    void add(const std::string exp);
private:
    std::unordered_map<std::string,TokenType> translate_table;
};