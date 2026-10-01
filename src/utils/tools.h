#pragma once
#include<string>
#include<vector>
#include"data_structure/keyword_table.h"
#include"data_structure/state.h"
#include"data_structure/token.h"
#include"data_structure/translate_table.h"
#include"utils/position.h"

void get_char(char&C,char*&forward);
void get_nbc(char&C,char*&forward,size_t&index);
void cat(const char C,std::string&token);
bool letter(const char C);
bool digit(const char C);
bool op(const char C);
void retract(char*&forward);
int SToI(const std::string &token);
float SToF(const std::string &token);
char peek(const std::string &src,size_t index,int offset=0);
void finish(TokenType type,const std::vector<Pos>positions,std::string &token,
    TranslateTable &transtable,size_t &index,State&state){
    int line = positions[index].line;
    int column = positions[index].column;
    transtable.add(TokenType::ShrAssign,token,line,column);
    index=index+token.size();
    token.clear();
    state = State::Start;
}

