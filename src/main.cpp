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
                HandleStart(state,forward,token,C,index,src);
                break;
            //关键词或标识符
            case State::InId:
                HandleInId(state,forward,token,C,index,positions,src,keytable,transtable);
                break;
            //数字
            case State::InNum:
                HandleInNum(state,forward,token,C,index,positions,src,transtable);
                break;
            //浮点数
            case State::InFloat: 
                HandleInFloat(state,forward,token,C,index,positions,src,transtable);
                break;
            //指数
            case State::InExp:
                HandleInExp(state,forward,token,C,index,positions,src,transtable);
                break;
            //字符常量
            case State::InChar:
                HandleInChar(state,forward,token,C,index,positions,src,transtable);
                break;
            //字符串常量
            case State::InStr:
                HandleInStr(state,forward,token,C,index,positions,src,transtable);
                break;
            //注释
            case State::InComment:
                HandleInComment(state,forward,token,C,index,positions,src,transtable);
                break;
            //预处理
            case State::InPreproc:
                HandleInPreproc(state,forward,token,C,index,positions,src,transtable);
                break;
            //运算符
            case State::InOp:
                HandleInOp(state,forward,token,C,index,positions,src,transtable);
            default:
                break;
        }
    }while(1);
    return 0;
}