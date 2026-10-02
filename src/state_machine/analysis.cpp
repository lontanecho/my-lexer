#include"state_machine/analysis.h"
#include"utils/tools.h"
#include<string_view>
#include<unordered_map>

namespace {

void setError(State& state, std::string& token, std::string& errorMessage,
    const char* message) {
    token.clear();
    errorMessage = message;
    state = State::Error;
}

bool validSimpleEscape(const char C) {
    return C=='\\'||C=='\''||C=='"'||C=='a'||C=='b'||C=='f'||
        C=='n'||C=='r'||C=='t'||C=='v'||C=='?'||C=='0';
}

bool isExponentMarker(const char C) {
    return C == 'e' || C == 'E';
}

bool isDirectiveStart(const std::string& src, const size_t index) {
    size_t i = index;
    while (true) {
        while (i > 0 && (src[i - 1] == ' ' || src[i - 1] == '\t' ||
            src[i - 1] == '\f' || src[i - 1] == '\v')) {
            --i;
        }
        if (i < 2 || src[i - 2] != '*' || src[i - 1] != '/') {
            break;
        }
        const size_t commentStart = src.rfind("/*", i - 2);
        if (commentStart == std::string::npos) {
            break;
        }
        i = commentStart;
    }
    if (i == 0 || src[i - 1] == '\n' || src[i - 1] == '\r') {
        // A backslash-newline pair continues the previous logical line.
        size_t lineEnd = i;
        if (lineEnd > 0 && src[lineEnd - 1] == '\n') {
            size_t previous = lineEnd - 1;
            if (previous > 0 && src[previous - 1] == '\r') {
                --previous;
            }
            while (previous > 0 && (src[previous - 1] == ' ' ||
                src[previous - 1] == '\t')) {
                --previous;
            }
            if (previous > 0 && src[previous - 1] == '\\') {
                return false;
            }
        }
        return true;
    }
    return false;
}

struct OperatorSpec {
    std::string_view lexeme;
    TokenType type;
};

void finishOperator(State& state, char*& forward, std::string& token,
    size_t& index, const std::vector<Pos>& positions, std::string& src,
    TranslateTable& transtable, std::string& errorMessage) {
    // 按长度降序排列，保证多字符运算符优先匹配。
    static constexpr OperatorSpec operators[] = {
        {"->*", TokenType::ArrowStar},
        {"<<=", TokenType::ShlAssign}, {">>=", TokenType::ShrAssign},
        {"++", TokenType::PlusPlus}, {"--", TokenType::MinusMinus},
        {"+=", TokenType::PlusAssign}, {"-=", TokenType::MinusAssign},
        {"*=", TokenType::StarAssign}, {"/=", TokenType::SlashAssign},
        {"%=", TokenType::PercentAssign}, {"==", TokenType::Eq},
        {"<=", TokenType::Le}, {">=", TokenType::Ge},
        {"&&", TokenType::And}, {"||", TokenType::Or},
        {"&=", TokenType::AndAssign}, {"|=", TokenType::OrAssign},
        {"^=", TokenType::XorAssign}, {"->", TokenType::Arrow},
        {"<<", TokenType::Shl}, {">>", TokenType::Shr},
        {"!=", TokenType::Ne}, {".*", TokenType::DotStar},
        {"::", TokenType::Scope},
        {"+", TokenType::Plus}, {"-", TokenType::Minus},
        {"*", TokenType::Star}, {"/", TokenType::Slash},
        {"%", TokenType::Percent}, {"=", TokenType::Assign},
        {"<", TokenType::Lt}, {">", TokenType::Gt},
        {"!", TokenType::Not}, {"&", TokenType::BitAnd},
        {"|", TokenType::BitOr}, {"^", TokenType::BitXor},
        {"~", TokenType::BitNot}, {"?", TokenType::Question},
        {".", TokenType::Dot}, {":", TokenType::Colon},
        {"(", TokenType::LParen}, {")", TokenType::RParen},
        {"{", TokenType::LBrace}, {"}", TokenType::RBrace},
        {"[", TokenType::LBracket}, {"]", TokenType::RBracket},
        {";", TokenType::Semicolon}, {",", TokenType::Comma}
    };

    for (const OperatorSpec& spec : operators) {
        if (src.compare(index, spec.lexeme.size(), spec.lexeme) == 0) {
            token.assign(spec.lexeme);
            forward = src.data() + index + spec.lexeme.size();
            finish(spec.type, positions, token, transtable, index, state);
            return;
        }
    }

    setError(state, token, errorMessage, "无法识别的运算符");
}

}

// 处理开始
void HandleStart(State&state,char*&forward,std::string&token,char &C,
    size_t &index,std::string &src,std::string& errorMessage){
    get_nbc(C,forward,index); //跳过空格、\t、\r、\n
    if(C=='\0'){//读到源文件末尾
        state = State::Done;
    }
    else if(letter(C)||C=='_'){
        state = State::InId; //标识符以字母或下划线开头
        cat(C,token);
    }
    else if(digit(C)){
        state = State::InNum; //数字以数字开头
        cat(C,token);
    }
    else if(C=='.' && digit(peek(src,index,1))){
        state = State::InFloat; //支持 .5 形式的浮点数
        cat(C,token);
    }
    else if(C=='\'') {
        state = State::InChar;
        cat(C,token);
    }
    else if(C=='"'){
        state = State::InStr;
        cat(C,token);
    }
    else if(C=='/'){
        if(peek(src,index,1)=='/'||peek(src,index,1)=='*'){
            state = State::InComment;
            cat(C,token);
        }
        else{
            state = State::InOp;
            cat(C,token);
        }
    }
    else if(C=='#' && isDirectiveStart(src,index)){
        state = State::InPreproc;
        cat(C,token);
    }
    else if(op(C)){
        state = State::InOp;
        cat(C,token);
    }
    else{
        setError(state,token,errorMessage,"非法字符");
    }
}

// 处理标识符或关键字
void HandleInId(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    const KeywordTable& keytable,TranslateTable &transtable,std::string& errorMessage){
    get_char(C,forward);
    if(letter(C)||digit(C)||C=='_'){
        cat(C,token);
    }
    else{
   
        if(keytable.find(token)!=TokenType::Error){//关键字
            finish(keytable.find(token),positions,token,transtable,index,state);
        }
        else{//标识符
            finish(TokenType::Identifier,positions,token,transtable,index,state);
        }
        retract(forward);
    }
}

// 处理数字
void HandleInNum(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if(C=='.'){
        state = State::InFloat;
        cat(C,token);
    }
    else if(isExponentMarker(C)){
        state = State::InExp;
        cat(C,token);
    }
    else{
        finish(TokenType::IntLiteral,positions,token,transtable,index,state);
        retract(forward);
    }
}

// 处理浮点数
void HandleInFloat(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
     get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if(isExponentMarker(C)){
        state = State::InExp;
        cat(C,token);
    }
    else{
        finish(TokenType::FloatLiteral,positions,token,transtable,index,state);
        retract(forward);
    } 
}

// 处理指数
void HandleInExp(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if((C=='+'||C=='-') && !token.empty() &&
        isExponentMarker(token.back())){
        cat(C,token);
    }
    else if(!token.empty() && (isExponentMarker(token.back())||
        token.back()=='+'||token.back()=='-')){
        setError(state,token,errorMessage,"指数部分必须包含数字");
    }
    else{
        finish(TokenType::FloatLiteral,positions,token,transtable,index,state);
        retract(forward);
    }    
}

// 处理字符
void HandleInChar(State&state,char*&forward,std::string&token,char &C,size_t &index,
    const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    get_char(C,forward);
    if(C=='\n'||C=='\r'||C=='\0'){//行尾或文件尾,字符常量未闭合
        setError(state,token,errorMessage,"字符常量未闭合");
    }
    else if(C=='\\'){//转义字符
        const char escaped = *forward;
        if(!validSimpleEscape(escaped)){
            setError(state,token,errorMessage,"字符常量中的转义序列非法");
            return;
        }
        cat(C,token);
        get_char(C,forward);
        cat(C,token);
        get_char(C,forward);
        if(C!='\''){
            setError(state,token,errorMessage,"字符常量必须只包含一个字符");
            return;
        }
        cat(C,token);
        finish(TokenType::CharLiteral,positions,token,transtable,index,state);
    }
    else if(C=='\''){//收尾单引号
        if(token.size()==2){
            cat(C,token);
            finish(TokenType::CharLiteral,positions,token,transtable,index,state);
        }
        else{//空字符常量
            setError(state,token,errorMessage,"字符常量必须只包含一个字符");
        }
    }
    else{//普通字符
        if(token.size()!=1){
            setError(state,token,errorMessage,"字符常量必须只包含一个字符");
        }
        else{
            cat(C,token);
        }
    }
}

// 处理字符串
void HandleInStr(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    get_char(C,forward);
    if(C=='\n'||C=='\r'||C=='\0'){//字符串未闭合
        setError(state,token,errorMessage,"字符串常量未闭合");
    }
    else if(C=='\\'){//转义字符
        cat(C,token);
        const char escaped = *forward;
        if(validSimpleEscape(escaped)){
            get_char(C,forward);
            cat(C,token);
        }
        else{//非法转义
            setError(state,token,errorMessage,"字符串常量中的转义序列非法");
        }
    }
    else if(C=='"'){
        cat(C,token);
        finish(TokenType::StringLiteral,positions,token,transtable,index,state);
    }
    else{
        cat(C,token);
    }
}

// 处理注释
void HandleInComment(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    if(*forward=='/'){//行注释:从'//'到行尾
        while(*forward!='\0'&&*forward!='\n'&&*forward!='\r'){
            forward++;
        }
        if(*forward=='\r'){
            forward++;
            if(*forward=='\n'){
                forward++;
            }
        }
        else if(*forward=='\n'){//跳过行尾换行符
            forward++;
        }
    }
    else if(*forward=='*'){//块注释:从'/*'到'*/'
        size_t commentIndex = static_cast<size_t>(forward - src.data()) + 1;
        while(commentIndex < src.size() &&
            !(src[commentIndex]=='*' && commentIndex + 1 < src.size() &&
                src[commentIndex + 1]=='/')){
            ++commentIndex;
        }
        if(commentIndex >= src.size()){//注释未闭合
            setError(state,token,errorMessage,"块注释未闭合");
            return;
        }
        forward = src.data() + commentIndex + 2;//跳过'*/'
    }
    else{//既不是'/'也不是'*'
        setError(state,token,errorMessage,"注释起始符非法");
        return;
    }
    index=forward-src.data();
    token.clear();
    state=State::Start;
}

// 处理预处理
void HandleInPreproc(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    //预处理指令表,token中已含'#'
    static const std::unordered_map<std::string,TokenType> directives={
        {"#include",TokenType::PpInclude},
        {"#define", TokenType::PpDefine},
        {"#undef",  TokenType::PpUndef},
        {"#if",     TokenType::PpIf},
        {"#ifdef",  TokenType::PpIfdef},
        {"#ifndef", TokenType::PpIfndef},
        {"#else",   TokenType::PpElse},
        {"#elif",   TokenType::PpElif},
        {"#endif",  TokenType::PpEndif},
        {"#pragma", TokenType::PpPragma},
        {"#error",  TokenType::PpError},
        {"#warning",TokenType::PpWarning},
        {"#line",   TokenType::PpLine}
    };
    if(token=="#"){
        while(*forward==' '||*forward=='\t'||*forward=='\f'||*forward=='\v'){
            ++forward;
        }
    }
    get_char(C,forward);
    if(letter(C)||digit(C)||C=='_'){//指令名由字母、数字、下划线组成
        cat(C,token);
    }
    else{//指令名结束,查表确定类型
        auto it=directives.find(token);
        if(it!=directives.end()){
            const size_t delimiterIndex = static_cast<size_t>(forward-src.data())-1;
            transtable.add(it->second,token,positions[index].line,positions[index].column);
            index = delimiterIndex;
            token.clear();
            state = State::Start;
        }
        else{//未知指令
            setError(state,token,errorMessage,"未知的预处理指令");
        }
        retract(forward);
    }
}

// 处理运算符
void HandleInOp(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>& positions,std::string &src,
    TranslateTable &transtable,std::string& errorMessage){
    finishOperator(state, forward, token, index, positions, src,
        transtable, errorMessage);
}
