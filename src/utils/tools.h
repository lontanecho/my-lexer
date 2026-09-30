#include<string>
#include"data_structure/keyword_table.h"
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

