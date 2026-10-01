#include"state_machine/analysis.h"

void HandleStart(State&state,char*&forward,std::string&token,char &C,
    size_t &index,const std::vector<Pos>Positions,std::string &src){
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