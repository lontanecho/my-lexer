#pragma once
#include<string>
#include<vector>
#include"data_structure/token.h"

class TranslateTable {
public:
    TranslateTable()=default;
    void add(const TokenType type,const std::string& token,const int line,const int column);
    void print()const;
private:
    std::vector<Token>table;
};
