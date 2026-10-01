#pragma once
#include<string>
#include<vector>
#include"data_structure/state.h"
#include"data_structure/token.h"
#include"data_structure/translate_table.h"
#include"data_structure/keyword_table.h"
#include"utils/tools.h"
#include"utils/position.h"


void HandleStart(State&state,char*&forword,std::string&token,char &C,
    size_t &index,std::string &src);

void HandleInId(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,const KeywordTable keytable,TranslateTable &transtable);

void HandleInNum(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInFloat(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInExp(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInStr(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInChar(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>perror,std::string &src,TranslateTable &transtable);

void HandleInComment(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInPreproc(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

void HandleInOp(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable);

