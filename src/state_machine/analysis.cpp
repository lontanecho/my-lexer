#include"state_machine/analysis.h"
#include"utils/tools.h"


void HandleStart(State&state,char*&forward,std::string&token,char &C,
    size_t &index,std::string &src){
    get_nbc(C,forward,index); //跳过空格、\t、\r、\n
    if(letter(C)||C=='_'){
        state = State::InId; //标识符以字母或下划线开头
        cat(C,token);
    }
    else if(digit(C)){
        state = State::InNum; //数字以数字开头
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
    else if(C=='#'){
        state = State::InPreproc;
        cat(C,token);
    }
    else if(op(C)){
        state = State::InOp;
        cat(C,token);
    }
    else{
        state = State::Error;
    }
}

void HandleInId(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,const KeywordTable&keytable,TranslateTable &transtable){ 
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

void HandleInNum(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){
    get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if(C=='.'){
        state = State::InFloat;
        cat(C,token);
    }
    else if(C=='e'||C=='E'){
        state = State::InExp;
        cat(C,token);
    }
    else{
        finish(TokenType::IntLiteral,positions,token,transtable,index,state);
        retract(forward);
    }
}

void HandleInFloat(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){ 
     get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if(C=='e'||C=='E'){
        state = State::InExp;
        cat(C,token);
    }
    else{
        finish(TokenType::FloatLiteral,positions,token,transtable,index,state);
        retract(forward);
    } 
}

void HandleInExp(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){
    get_char(C,forward);
    if(digit(C)){
        cat(C,token);
    }
    else if(C=='+'||C=='-'){
        cat(C,token);
    }
    else{
        finish(TokenType::IntLiteral,positions,token,transtable,index,state);
        retract(forward);
    }    
}

void HandleInChar(State&state,char*&forward,std::string&token,char &C,size_t &index,
    const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){
    bool iscatch=false;
    get_char(C,forward);
    if(C=='\n'||C=='\r'||C==EOF){
        token.clear();
        state = State::Error;
    }
    else if(C=='\\'){
        cat(C,token);
        char chf=peek(src,index,1);
        char chs=peek(src,index,2);
        if((chf=='\\'||chf=='\''||chf=='\"'||chf=='a'||chf=='b'||chf=='f'||
            chf=='n'||chf=='r'||chf=='t'||chf=='v'||chf=='?')&&(chs=='\'')){
            cat(chf,token);
            cat(chs,token);
            get_char(C,forward);
            get_char(C,forward);
            finish(TokenType::CharLiteral,positions,token,transtable,index,state);
        } 
        else{
            token.clear();
            state = State::Error;
        }
    }
    else if(C=='\''){
        if(iscatch){
            finish(TokenType::CharLiteral,positions,token,transtable,index,state);
        }
        else{
            token.clear();
            state = State::Error;
        }
    }
    else {
        iscatch=true;
        cat(C,token);
    }
}

void HandleInStr(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){ 
    get_char(C,forward);
    if(C=='\n'||C=='\r'||C==EOF){
        token.clear();
        state = State::Error;
    }
    else if(C=='\\'){
        cat(C,token);
        char chf=peek(src,index,1);
        char chs=peek(src,index,2);
        if((chf=='\\'||chf=='\''||chf=='\"'||chf=='a'||chf=='b'||chf=='f'||
            chf=='n'||chf=='r'||chf=='t'||chf=='v'||chf=='?')&&(chs=='\'')){
            cat(chf,token);
            cat(chs,token);
            get_char(C,forward);
            get_char(C,forward);
        } 
        else{
            token.clear();
            state = State::Error;
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

void HandleInComment(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){ 
    get_char(C,forward);
    cat(C,token);
    if(C=='/'){
        while(C!='\n'){
            get_char(C,forward);
            cat(C,token);
        }
        state=State::Start;
        index=index+token.size();
        token.clear();
    }
    else if(C=='*'){
        while(C!=EOF){
            while(C!='*'){
                get_char(C,forward);
                cat(C,token);
            }
            get_char(C,forward);
            cat(C,token);
            if(C=='/'){
                state=State::Start;
                index=index+token.size();
                token.clear();
            }
        }
    }
}

void HandleInPreproc(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){ 
    
}

void HandleInOp(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>positions,std::string &src,TranslateTable &transtable){ 
    //'<','<=','<<','<<='
    if(C=='<'){
        get_char(C,forward);
        if(C=='='){
            cat(C,token);
            finish(TokenType::Le,positions,token,transtable,index,state);
        }
        else if(C=='<'){
            cat(C,token);
            get_char(C,forward);
            if(C=='='){
                cat(C,token);
                finish(TokenType::ShlAssign,positions,token,transtable,index,state);
            }
            else{
                finish(TokenType::Shl,positions,token,transtable,index,state);
                retract(forward);
            }
        }
        else{
            finish(TokenType::Lt,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    //'>','>=','>>','>>='
    else if(C=='>'){
            get_char(C,forward);
        if(C=='='){
            cat(C,token);
            finish(TokenType::Ge,positions,token,transtable,index,state);
        }
        else if(C=='<'){
            cat(C,token);
            get_char(C,forward);
            if(C=='='){
                cat(C,token);
                finish(TokenType::ShrAssign,positions,token,transtable,index,state);
            }
            else{
                finish(TokenType::Shr,positions,token,transtable,index,state);
                retract(forward);
            }
        }
        else{
            finish(TokenType::Gt,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    //'=='、'='
    else if(C=='='){
        get_char(C,forward);
        if(C=='='){
            cat(C,token);
            finish(TokenType::Eq,positions,token,transtable,index,state);
        }
        else{
            finish(TokenType::Assign,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    //'!='、'!'
    else if(C=='!'){
        get_char(C,forward);
        if(C=='='){
            cat(C,token);
            finish(TokenType::Ne,positions,token,transtable,index,state);
        }
        else{
            finish(TokenType::Not,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    //'&'、'&='、'&&'
    else if(C=='&'){
        get_char(C,forward);
        if(C=='&'){
            cat(C,token);
            finish(TokenType::And,positions,token,transtable,index,state);
        }
        else if(C=='='){
            cat(C,token);
            finish(TokenType::AndAssign,positions,token,transtable,index,state);
        }
        else{
            finish(TokenType::BitAnd,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    else if(C=='|'){
        get_char(C,forward);
        if(C=='|'){
            cat(C,token);
            finish(TokenType::Or,positions,token,transtable,index,state);
        }
        else if(C=='='){
            cat(C,token);
            finish(TokenType::OrAssign,positions,token,transtable,index,state);
        }
        else{
            finish(TokenType::BitOr,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    else if(C=='^'){
        get_char(C,forward);
        if(C=='='){
            cat(C,token);
            finish(TokenType::XorAssign,positions,token,transtable,index,state);  
        }
        else{
            finish(TokenType::BitXor,positions,token,transtable,index,state);
            retract(forward);
        }
    }
    else if(C=='~'){ 
        finish(TokenType::BitNot,positions,token,transtable,index,state);
    }
    else if(C=='?'){ 
        
    }
}
