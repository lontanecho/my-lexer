#include<string>
#include"data_structure/keyword_table.h"
void get_char(char&C,const std::string&souce_file,char*&forward);
void get_nbc(char&C,const std::string&souce_file,char*&forward);
void cat(const char C,std::string&token);
bool letter(const char C);
bool digit(const char C);
void retract(char*&forword);
int SToI(const std::string &token);
float SToF(const std::string &token);

