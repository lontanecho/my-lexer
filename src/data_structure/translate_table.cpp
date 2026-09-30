#include"data_structure/translate_table.h"

void TranslateTable::add(const TokenType type,const std::string token,const int line,const int column){
    table.push_back(Token{type,token,line,column});
}