#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include"data_structure/keyword_table.h"
#include"utils/tools.h"
#include"utils/position.h"
enum class State {
    Start,  // 初始状态
    InId,   // 关键字或标识符
    InNum,  // 数字
    InFloat,    // 浮点数
    InExp,  // 指数
    InStr,  // 字符串
    InChar, // 单字符
    InComment,  // 注释
    InOp,   // 运算符
    InPreproc,  // 预处理
    Done,   // 结束
    Error   // 错误
}; 
char C;
int iskey;
int index=0;//源文件索引
std::string token;
char* lexemebegin;
char* forward;
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
    lexemebegin = src.data();
    forward = lexemebegin;
    KeywordTable keytable;
    std::vector<Pos> positions = buildPositions(src);
    

    do{
        switch(state){
            case State::Start: 
                get_nbc(C,forward); //跳过空格、\t、\r、\n
                if(letter(C)||C=='_'){
                    state = State::InId; //标识符以字母或下划线开头
                }
                else if(digit(C)){
                    state = State::InNum; //数字以数字开头
                }
                else if(C=='\'') {
                    state = State::InChar;
                }
                else if(C=='"'){
                    state = State::InStr;
                }
                else if(C=='/'){
                    if(peek(src,index,1)=='/'||peek(src,index,1)=='*'){
                        state = State::InComment;
                    }
                    else{
                        state = State::InOp;
                    }
                }
                else if(C=='#'){
                    state = State::InPreproc;
                }
                else if(op(C)){
                    state = State::InOp;
                }
                else{
                    state = State::Error;
                }
                break;
            case State::InId:
            default:
                break;
        }
    }while(1);
    return 0;
}