#include "data_structure/keyword_table.h"

KeywordTable ::KeywordTable():table({
        {"int",1},{"char",2},{"float",3},{"double",4},{"bool",5},
        {"void",6},{"if",7},{"else",8},{"switch",9},{"case",10},
        {"default",11},{"for",12},{"while",13},{"do",14},{"break",15},
        {"continue",16},{"return",17},{"class",18},{"struct",19},{"enum",20},
        {"private",21},{"public",22},{"protected",23},{"virtual",24},{"this",25},
        {"friend",26},{"operator",27},{"const",28},{"static",29},{"new",30},
        {"delete",31},{"true",32},{"false",33},{"nullptr",34}
    }){}


int KeywordTable::find(const std::string &lexeme)const{
    auto it=table.find(lexeme);
    return it==table.end()?-1:it->second;
}