#include<iostream>
#include<algorithm>
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
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string src = buffer.str();

    //初始化
    char C = '\0';
    size_t sourceIndex = 0;//源文件索引,跳过空格注释,匹配成功时更新
    State state=State::Start;
    KeywordTable keytable;
    char* forward = src.data();
    std::string token;
    std::vector<Pos> positions = buildPositions(src);
    TranslateTable transtable;
    std::string errorMessage;
    bool hadError = false;
    
    do{
        switch(state){
            //开始
            case State::Start: 
                HandleStart(state,forward,token,C,sourceIndex,src,errorMessage);
                break;
            //关键词或标识符
            case State::InId:
                HandleInId(state,forward,token,C,sourceIndex,positions,src,keytable,transtable,errorMessage);
                break;
            //数字
            case State::InNum:
                HandleInNum(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //浮点数
            case State::InFloat: 
                HandleInFloat(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //指数
            case State::InExp:
                HandleInExp(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //字符常量
            case State::InChar:
                HandleInChar(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //字符串常量
            case State::InStr:
                HandleInStr(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //注释
            case State::InComment:
                HandleInComment(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //预处理
            case State::InPreproc:
                HandleInPreproc(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //运算符
            case State::InOp:
                HandleInOp(state,forward,token,C,sourceIndex,positions,src,transtable,errorMessage);
                break;
            //词法错误
            case State::Error:
                hadError = true;
                sourceIndex = std::min(sourceIndex, src.size());
                std::cerr<<"词法错误: 第"<<positions[sourceIndex].line
                    <<"行 第"<<positions[sourceIndex].column<<"列"
                    <<"，"<<(errorMessage.empty()?"无法识别当前词素":errorMessage)
                    <<std::endl;
                state = State::Done;
                break;
            //结束
            case State::Done:
                break;
            default:
                state = State::Done;
                break;
        }
    }while(state!=State::Done);
    if(!hadError){
        transtable.add(TokenType::Eof,"",positions[sourceIndex].line,positions[sourceIndex].column);
    }
    transtable.print();
    return hadError ? 1 : 0;
}
