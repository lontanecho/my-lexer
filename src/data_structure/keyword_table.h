#include<string>
#include<unordered_map>
#include"data_structure/token.h"

class KeywordTable{
public:
    KeywordTable();
    TokenType find(const std::string &lexeme)const;
private:
    const std::unordered_map<std::string ,TokenType> table;
};
