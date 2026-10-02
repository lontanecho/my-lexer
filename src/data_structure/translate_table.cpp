#include"data_structure/translate_table.h"
#include<algorithm>
#include<iomanip>
#include<iostream>
#include<string>

// 词法单元类型 -> 名称,便于阅读输出
static const char* typeName(const TokenType type){
    switch(type){
        case TokenType::Eof:          return "Eof";
        case TokenType::Error:        return "Error";

        case TokenType::KwInt:        return "KwInt";
        case TokenType::KwChar:       return "KwChar";
        case TokenType::KwFloat:      return "KwFloat";
        case TokenType::KwDouble:     return "KwDouble";
        case TokenType::KwBool:       return "KwBool";
        case TokenType::KwVoid:       return "KwVoid";
        case TokenType::KwShort:      return "KwShort";
        case TokenType::KwLong:       return "KwLong";
        case TokenType::KwSigned:     return "KwSigned";
        case TokenType::KwUnsigned:   return "KwUnsigned";
        case TokenType::KwIf:         return "KwIf";
        case TokenType::KwElse:       return "KwElse";
        case TokenType::KwSwitch:     return "KwSwitch";
        case TokenType::KwCase:       return "KwCase";
        case TokenType::KwDefault:    return "KwDefault";
        case TokenType::KwFor:        return "KwFor";
        case TokenType::KwWhile:      return "KwWhile";
        case TokenType::KwDo:         return "KwDo";
        case TokenType::KwBreak:      return "KwBreak";
        case TokenType::KwContinue:   return "KwContinue";
        case TokenType::KwReturn:     return "KwReturn";
        case TokenType::KwGoto:       return "KwGoto";
        case TokenType::KwClass:      return "KwClass";
        case TokenType::KwStruct:     return "KwStruct";
        case TokenType::KwUnion:      return "KwUnion";
        case TokenType::KwEnum:       return "KwEnum";
        case TokenType::KwPublic:     return "KwPublic";
        case TokenType::KwPrivate:    return "KwPrivate";
        case TokenType::KwProtected:  return "KwProtected";
        case TokenType::KwVirtual:    return "KwVirtual";
        case TokenType::KwFriend:     return "KwFriend";
        case TokenType::KwThis:       return "KwThis";
        case TokenType::KwOperator:   return "KwOperator";
        case TokenType::KwConst:      return "KwConst";
        case TokenType::KwStatic:     return "KwStatic";
        case TokenType::KwExtern:     return "KwExtern";
        case TokenType::KwInline:     return "KwInline";
        case TokenType::KwMutable:    return "KwMutable";
        case TokenType::KwVolatile:   return "KwVolatile";
        case TokenType::KwNamespace:  return "KwNamespace";
        case TokenType::KwTemplate:   return "KwTemplate";
        case TokenType::KwTypename:   return "KwTypename";
        case TokenType::KwUsing:      return "KwUsing";
        case TokenType::KwTypedef:    return "KwTypedef";
        case TokenType::KwNew:        return "KwNew";
        case TokenType::KwDelete:     return "KwDelete";
        case TokenType::KwTry:        return "KwTry";
        case TokenType::KwCatch:      return "KwCatch";
        case TokenType::KwThrow:      return "KwThrow";
        case TokenType::KwTrue:       return "KwTrue";
        case TokenType::KwFalse:      return "KwFalse";
        case TokenType::KwNullptr:    return "KwNullptr";
        case TokenType::KwSizeof:     return "KwSizeof";

        case TokenType::Identifier:    return "Identifier";
        case TokenType::IntLiteral:    return "IntLiteral";
        case TokenType::FloatLiteral:  return "FloatLiteral";
        case TokenType::CharLiteral:   return "CharLiteral";
        case TokenType::StringLiteral: return "StringLiteral";

        case TokenType::Plus:         return "Plus";
        case TokenType::Minus:        return "Minus";
        case TokenType::Star:         return "Star";
        case TokenType::Slash:        return "Slash";
        case TokenType::Percent:      return "Percent";
        case TokenType::PlusPlus:     return "PlusPlus";
        case TokenType::MinusMinus:   return "MinusMinus";
        case TokenType::Eq:           return "Eq";
        case TokenType::Ne:           return "Ne";
        case TokenType::Lt:           return "Lt";
        case TokenType::Gt:           return "Gt";
        case TokenType::Le:           return "Le";
        case TokenType::Ge:           return "Ge";
        case TokenType::And:          return "And";
        case TokenType::Or:           return "Or";
        case TokenType::Not:          return "Not";
        case TokenType::BitAnd:       return "BitAnd";
        case TokenType::BitOr:        return "BitOr";
        case TokenType::BitXor:       return "BitXor";
        case TokenType::BitNot:       return "BitNot";
        case TokenType::Shl:          return "Shl";
        case TokenType::Shr:          return "Shr";
        case TokenType::Assign:       return "Assign";
        case TokenType::PlusAssign:   return "PlusAssign";
        case TokenType::MinusAssign:  return "MinusAssign";
        case TokenType::StarAssign:   return "StarAssign";
        case TokenType::SlashAssign:  return "SlashAssign";
        case TokenType::PercentAssign:return "PercentAssign";
        case TokenType::AndAssign:    return "AndAssign";
        case TokenType::OrAssign:     return "OrAssign";
        case TokenType::XorAssign:    return "XorAssign";
        case TokenType::ShlAssign:    return "ShlAssign";
        case TokenType::ShrAssign:    return "ShrAssign";
        case TokenType::Arrow:        return "Arrow";
        case TokenType::Dot:          return "Dot";
        case TokenType::ArrowStar:    return "ArrowStar";
        case TokenType::DotStar:      return "DotStar";
        case TokenType::Scope:        return "Scope";
        case TokenType::Question:     return "Question";

        case TokenType::LParen:    return "LParen";
        case TokenType::RParen:    return "RParen";
        case TokenType::LBracket:  return "LBracket";
        case TokenType::RBracket:  return "RBracket";
        case TokenType::LBrace:    return "LBrace";
        case TokenType::RBrace:    return "RBrace";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Comma:     return "Comma";
        case TokenType::Colon:     return "Colon";

        case TokenType::PpInclude: return "PpInclude";
        case TokenType::PpDefine:  return "PpDefine";
        case TokenType::PpUndef:   return "PpUndef";
        case TokenType::PpIf:      return "PpIf";
        case TokenType::PpIfdef:   return "PpIfdef";
        case TokenType::PpIfndef:  return "PpIfndef";
        case TokenType::PpElse:    return "PpElse";
        case TokenType::PpElif:    return "PpElif";
        case TokenType::PpEndif:   return "PpEndif";
        case TokenType::PpPragma:  return "PpPragma";
        case TokenType::PpError:   return "PpError";
        case TokenType::PpWarning: return "PpWarning";
        case TokenType::PpLine:    return "PpLine";
    }
    return "Unknown";
}

void TranslateTable::add(const TokenType type,const std::string& token,const int line,const int column){
    table.push_back(Token{type,token,line,column});
}

void TranslateTable::print()const{
    const std::string typeHeader = "类型";
    const std::string lineHeader = "行";
    const std::string columnHeader = "列";
    const std::string lexemeHeader = "词素";

    size_t typeWidth = typeHeader.size();
    size_t lineWidth = lineHeader.size();
    size_t columnWidth = columnHeader.size();
    size_t lexemeWidth = lexemeHeader.size();

    for(const Token &t:table){
        typeWidth = std::max(typeWidth, std::string(typeName(t.type)).size());
        lineWidth = std::max(lineWidth, std::to_string(t.line).size());
        columnWidth = std::max(columnWidth, std::to_string(t.column).size());
        lexemeWidth = std::max(lexemeWidth, t.lexeme.size());
    }

    std::cout<<std::left
        <<std::setw(static_cast<int>(typeWidth))<<typeHeader<<"  "
        <<std::right<<std::setw(static_cast<int>(lineWidth))<<lineHeader<<"  "
        <<std::setw(static_cast<int>(columnWidth))<<columnHeader<<"  "
        <<std::left<<std::setw(static_cast<int>(lexemeWidth))<<lexemeHeader
        <<std::endl;

    for(const Token &t:table){
        std::cout<<std::left
            <<std::setw(static_cast<int>(typeWidth))<<typeName(t.type)<<"  "
            <<std::right<<std::setw(static_cast<int>(lineWidth))<<t.line<<"  "
            <<std::setw(static_cast<int>(columnWidth))<<t.column<<"  "
            <<std::left<<std::setw(static_cast<int>(lexemeWidth))<<t.lexeme
            <<std::endl;
    }
}
