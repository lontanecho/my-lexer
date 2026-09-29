#include"utils/tools.h"
void get_char(char&C,const std::string&souce_file,char*&forward){
    C=*forward;
    forward++;
}
void get_nbc(char&C,const std::string&souce_file,char*&forward){
    while(C==' '||C=='\t'||C=='\n'||C=='\r'){
        forward++;
    }
    C=*forward;
    forward++;
}

void cat(const char C,std::string&token){
    token+=C;
}
bool letter(const char C){
    return (C>='a'&&C<='z')||(C>='A'&&C<='Z');
}
bool digit(const char C){
    return (C>='0'&&C<='9');
}
void retract(char*&forword){
    forword--;
}
int reserve(const std::string&lexeme,const KeywordTable&keytable){
    return keytable.find(lexeme);
}
int SToI(const std::string &token){
    return std::stoi(token);
}
float SToF(const std::string &token){
    return std::stof(token);
}

