#include "data_structure/keyword_table.h"
KeywordTable ::KeywordTable():table({
        {"int",TokenType::KwInt},{"char",TokenType::KwChar},{"float",TokenType::KwFloat},{"double",TokenType::KwDouble},{"bool",TokenType::KwBool},
        {"void",TokenType::KwVoid},{"if",TokenType::KwIf},{"else",TokenType::KwElse},{"switch",TokenType::KwSwitch},{"case",TokenType::KwCase},
        {"default",TokenType::KwDefault},{"for",TokenType::KwFor},{"while",TokenType::KwWhile},{"do",TokenType::KwDo},{"break",TokenType::KwBreak},
        {"continue",TokenType::KwContinue},{"return",TokenType::KwReturn},{"class",TokenType::KwClass},{"struct",TokenType::KwStruct},{"enum",TokenType::KwEnum},
        {"private",TokenType::KwPrivate},{"public",TokenType::KwPublic},{"protected",TokenType::KwProtected},{"virtual",TokenType::KwVirtual},{"this",TokenType::KwThis},
        {"friend",TokenType::KwFriend},{"operator",TokenType::KwOperator},{"const",TokenType::KwConst},{"static",TokenType::KwStatic},{"new",TokenType::KwNew},
        {"delete",TokenType::KwDelete},{"true",TokenType::KwTrue},{"false",TokenType::KwFalse},{"nullptr",TokenType::KwNullptr}
    }){}


TokenType KeywordTable::find(const std::string &lexeme)const{
    auto it=table.find(lexeme);
    return it==table.end()?TokenType::Error:it->second;
}