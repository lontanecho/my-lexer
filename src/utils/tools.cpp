#include"utils/tools.h"
void get_char(char&C,char*&forward){
    C=*forward;
    forward++;
}
void get_nbc(char&C,char*&forward){
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

bool op(const char C){
    return C=='+'||C=='-'||C=='*'||C=='/'||C=='%'||C=='='||C=='<'||
    C=='>'||C=='!'||C=='&'||C=='|'||C=='^'||C=='~'||C=='?'||C==':'||
    C=='.'||C=='#'||C=='@'||C=='$'||C=='%'||C=='^'||C==','||C==';'||
    C=='('||C==')'||C=='['||C==']'||C=='{'||C=='}';
}
void retract(char*&forword){
    forword--;
}
int SToI(const std::string &token){
    return std::stoi(token);
}
float SToF(const std::string &token){
    return std::stof(token);
}

char peek(const std::string &src, size_t index, int offset = 0){
    size_t i = index + offset;
    if (i >= src.size()) return '\0';
    return src[i];
}

