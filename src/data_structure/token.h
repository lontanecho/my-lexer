#include<string>

enum class TokenType : int {
    // 结束与错误 
    Eof = 0,
    Error = -1,

    //关键字
    KwInt = 1, KwChar, KwFloat, KwDouble, KwBool, KwVoid,
    KwShort, KwLong, KwSigned, KwUnsigned,
    KwIf, KwElse, KwSwitch, KwCase, KwDefault,
    KwFor, KwWhile, KwDo, KwBreak, KwContinue, KwReturn, KwGoto,
    KwClass, KwStruct, KwUnion, KwEnum,
    KwPublic, KwPrivate, KwProtected,
    KwVirtual, KwFriend, KwThis, KwOperator,
    KwConst, KwStatic, KwExtern, KwInline, KwMutable, KwVolatile,
    KwNamespace, KwTemplate, KwTypename, KwUsing, KwTypedef,
    KwNew, KwDelete, KwTry, KwCatch, KwThrow,
    KwTrue, KwFalse, KwNullptr, KwSizeof,

    // 标识符
    Identifier = 101,

    // 常量
    IntLiteral = 102,
    FloatLiteral = 103,
    CharLiteral = 104,
    StringLiteral = 105,

    // 运算符
    Plus = 201, Minus, Star, Slash, Percent,
    PlusPlus, MinusMinus,
    Eq, Ne, Lt, Gt, Le, Ge,
    And, Or, Not,
    BitAnd, BitOr, BitXor, BitNot,
    Shl, Shr,
    Assign, PlusAssign, MinusAssign, StarAssign, SlashAssign,
    PercentAssign, AndAssign, OrAssign, XorAssign, ShlAssign, ShrAssign,
    Arrow, Dot, ArrowStar, DotStar, Scope, Question,

    // 界符
    LParen = 301, RParen,
    LBracket, RBracket,
    LBrace, RBrace,
    Semicolon, Comma, Colon,

    // 预处理
    PpInclude = 401, PpDefine, PpUndef,
    PpIf, PpIfdef, PpIfndef, PpElse, PpElif, PpEndif,
    PpPragma, PpError, PpWarning, PpLine
};

struct Token{
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

