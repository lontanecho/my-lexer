#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include"data_structure/keyword_table.h"
#include"data_structure/translate_table.h"
#include"data_structure/state.h"
#include"utils/tools.h"
#include"utils/position.h"
#include"state_machine/analysis.h"

char C;
size_t index=0;//源文件索引,跳过空格注释,匹配成功时更新
char* forward;
std::string token;
std::stringstream buffer;


int main(int argc, char*argv[]){
    //输入源程序文件
    if(argc<2){
        std::cerr<<"用法:"<<argv[0]<<" <文件名>"<<std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if(!file.is_open()){
        std::cerr<<"无法打开文件:"<<argv[1]<<std::endl;
        return 1;
    }
    buffer << file.rdbuf();
    std::string src = buffer.str();

    //初始化
    State state=State::Start;
    KeywordTable keytable;
    forward = src.data();
    std::vector<Pos> positions = buildPositions(src);
    TranslateTable transtable;
    
    do{
        switch(state){
            //开始
            case State::Start: 
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
                break;
            //关键词或标识符
            case State::InId:
                get_char(C,forward);
                if(letter(C)||digit(C)||C=='_'){
                    cat(C,token);
                }
                else{
                    int line = positions[index].line;
                    int column = positions[index].column;
                    if(keytable.find(token)!=TokenType::Error){//关键字
                        transtable.add(keytable.find(token),token,line,column);
                    }
                    else{//标识符
                        transtable.add(TokenType::Identifier,token,line,column);
                    }
                    index=index+token.size();
                    retract(forward);
                    state = State::Start;
                    token.clear();
                }
                break;
            //数字
            case State::InNum:
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
                    int line = positions[index].line;
                    int column = positions[index].column;
                    transtable.add(TokenType::IntLiteral,token,line,column);
                    index=index+token.size();
                    retract(forward);
                    state = State::Start;
                    token.clear();
                }
                break;
            //浮点数
            case State::InFloat: 
                get_char(C,forward);
                if(digit(C)){
                    cat(C,token);
                }
                else if(C=='e'||C=='E'){
                    state = State::InExp;
                    cat(C,token);
                }
                else{
                    int line = positions[index].line;
                    int column = positions[index].column;
                    transtable.add(TokenType::FloatLiteral,token,line,column);
                    index=index+token.size();
                    retract(forward);
                    state = State::Start;
                    token.clear();
                }
                break;
            //指数
            case State::InExp:
                get_char(C,forward);
                if(digit(C)){
                    cat(C,token);
                }
                else if(C=='+'||C=='-'){
                    cat(C,token);
                }
                else{
                    int line = positions[index].line;
                    int column = positions[index].column;
                    transtable.add(TokenType::FloatLiteral,token,line,column);
                    index=index+token.size();
                    retract(forward);
                    state = State::Start;
                    token.clear();
                }
                break;
            //字符常量
            case State::InChar:
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
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::CharLiteral,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    } 
                    else{
                        token.clear();
                        state = State::Error;
                    }
                }
                else if(C=='\''){
                    if(iscatch){
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::CharLiteral,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
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
                break;
            //字符串常量
            case State::InStr:
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
                    int line = positions[index].line;
                    int column = positions[index].column;
                    transtable.add(TokenType::StringLiteral,token,line,column);
                    index=index+token.size();
                    token.clear();
                    state = State::Start;
                }
                else{
                    cat(C,token);
                }
                break;
            //注释
            case State::InComment:
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
                break;
            //运算符
            case State::InOp:
                //'<','<=','<<','<<='
                if(C=='<'){
                    get_char(C,forward);
                    if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Le,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else if(C=='<'){
                        cat(C,token);
                        get_char(C,forward);
                        if(C=='='){
                            cat(C,token);
                            int line = positions[index].line;
                            int column = positions[index].column;
                            transtable.add(TokenType::ShlAssign,token,line,column);
                            index=index+token.size();
                            token.clear();
                            state = State::Start;
                        }
                        else{
                            int line = positions[index].line;
                            int column = positions[index].column;
                            transtable.add(TokenType::Shl,token,line,column);
                            index=index+token.size();
                            token.clear();
                            retract(forward);
                            state = State::Start;
                        }
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Lt,token,line,column);
                        index=index+token.size();
                        token.clear();
                        retract(forward);
                        state = State::Start;
                    }
                }
                else if(C=='>'){
                     get_char(C,forward);
                    if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Ge,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else if(C=='<'){
                        cat(C,token);
                        get_char(C,forward);
                        if(C=='='){
                            cat(C,token);
                            int line = positions[index].line;
                            int column = positions[index].column;
                            transtable.add(TokenType::ShrAssign,token,line,column);
                            index=index+token.size();
                            token.clear();
                            state = State::Start;
                        }
                        else{
                            int line = positions[index].line;
                            int column = positions[index].column;
                            transtable.add(TokenType::Shr,token,line,column);
                            index=index+token.size();
                            token.clear();
                            retract(forward);
                            state = State::Start;
                        }
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Gt,token,line,column);
                        index=index+token.size();
                        token.clear();
                        retract(forward);
                        state = State::Start;
                    }
                }
                else if(C=='='){
                    get_char(C,forward);
                    if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Eq,token,line,column);
                        index=index+token.size();
                        token.clear();
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Assign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        retract(forward);
                        state = State::Start;
                    }
                }
                else if(C=='!'){
                    get_char(C,forward);
                    if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::OrAssign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Not,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                        retract(forward);
                    }
                }
                else if(C=='&'){
                    get_char(C,forward);
                    if(C=='&'){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::And,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::AndAssign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else if(C=='*'){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::AndAssign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::BitAnd,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                        retract(forward);
                    }
                }
                else if(C=='|'){
                    get_char(C,forward);
                    if(C=='|'){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::Or,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::OrAssign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::BitOr,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                        retract(forward);
                    }
                }
                else if(C=='^'){
                    get_char(C,forward);
                    if(C=='='){
                        cat(C,token);
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::XorAssign,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;   
                    }
                    else{
                        int line = positions[index].line;
                        int column = positions[index].column;
                        transtable.add(TokenType::BitXor,token,line,column);
                        index=index+token.size();
                        token.clear();
                        state = State::Start;
                        retract(forward);
                    }
                }
                else if(C=='~'){ 
                    int line = positions[index].line;
                    int column = positions[index].column;
                    transtable.add(TokenType::BitNot,token,line,column);
                    index=index+token.size();
                    token.clear();
                    state = State::Start;
                }
                else if(C=='?'){ 
                    
                }
            default:
                break;
        }
    }while(1);
    return 0;
}