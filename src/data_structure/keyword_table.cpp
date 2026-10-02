#include "data_structure/keyword_table.h"
KeywordTable ::KeywordTable():table({
        {"int",TokenType::KwInt},{"char",TokenType::KwChar},{"float",TokenType::KwFloat},{"double",TokenType::KwDouble},{"bool",TokenType::KwBool},
        {"void",TokenType::KwVoid},{"short",TokenType::KwShort},{"long",TokenType::KwLong},{"signed",TokenType::KwSigned},{"unsigned",TokenType::KwUnsigned},
        {"if",TokenType::KwIf},{"else",TokenType::KwElse},{"switch",TokenType::KwSwitch},{"case",TokenType::KwCase},
        {"default",TokenType::KwDefault},{"for",TokenType::KwFor},{"while",TokenType::KwWhile},{"do",TokenType::KwDo},{"break",TokenType::KwBreak},
        {"continue",TokenType::KwContinue},{"return",TokenType::KwReturn},{"goto",TokenType::KwGoto},{"class",TokenType::KwClass},
        {"struct",TokenType::KwStruct},{"union",TokenType::KwUnion},{"enum",TokenType::KwEnum},
        {"private",TokenType::KwPrivate},{"public",TokenType::KwPublic},{"protected",TokenType::KwProtected},{"virtual",TokenType::KwVirtual},{"this",TokenType::KwThis},
        {"friend",TokenType::KwFriend},{"operator",TokenType::KwOperator},{"const",TokenType::KwConst},{"static",TokenType::KwStatic},
        {"extern",TokenType::KwExtern},{"inline",TokenType::KwInline},{"mutable",TokenType::KwMutable},{"volatile",TokenType::KwVolatile},
        {"namespace",TokenType::KwNamespace},{"template",TokenType::KwTemplate},{"typename",TokenType::KwTypename},
        {"using",TokenType::KwUsing},{"typedef",TokenType::KwTypedef},{"new",TokenType::KwNew},{"delete",TokenType::KwDelete},
        {"try",TokenType::KwTry},{"catch",TokenType::KwCatch},{"throw",TokenType::KwThrow},
        {"true",TokenType::KwTrue},{"false",TokenType::KwFalse},{"nullptr",TokenType::KwNullptr},{"sizeof",TokenType::KwSizeof}
    }){}


TokenType KeywordTable::find(const std::string &lexeme)const{
    auto it=table.find(lexeme);
    return it==table.end()?TokenType::Error:it->second;
}
