#include"data_structure/symbol_table.h"

SymbolTable::SymbolTable():id(0){}

int SymbolTable::find(const std::string &lexeme)const{
    auto it=table.find(lexeme);
    return it==table.end()?-1:it->second;
}

void SymbolTable::add(const std::string &lexeme){
    auto it=table.find(lexeme);
    if(it!=table.end())return;
    table.insert(std::make_pair(lexeme,id));
    id++;
}
