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
    size_t &index,std::string &src,std::string& errorMessage);

void HandleInId(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    const KeywordTable& keytable,TranslateTable &transtable,std::string& errorMessage);

void HandleInNum(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInFloat(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInExp(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInStr(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInChar(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInComment(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInPreproc(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

void HandleInOp(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage);

