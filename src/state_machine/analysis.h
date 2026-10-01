#pragma once
#include<string>
#include<vector>
#include"data_structure/state.h"
#include"data_structure/token.h"
#include"data_structure/translate_table.h"
#include"utils/tools.h"
#include"utils/position.h"

void HandleStart(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>Positions,std::string &src);

void HandleInId(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>Positions,std::string &src);

void HandleInNum(State&state,char*&forword,std::string&token,char &C,
    size_t &index,const std::vector<Pos>Positions,std::string &src);