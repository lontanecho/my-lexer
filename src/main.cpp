#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include"data_structure/keyword_table.h"
#include"utils/tools.h"
#include"utils/position.h"
enum class State {
    Start,
    InId,
    InNum,
    InFloat,
    InExp,
    InStr,
    InChar,
    InComment,
    InOp,
    InPreproc,
    Done,
    Error
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
    std::string source_file = buffer.str();
    //初始化
    State state=State::Start;
    lexemebegin = source_file.data();
    forward = lexemebegin;
    KeywordTable keytable;
    std::vector<Pos> positions = buildPositions(source_file);
    

    do{
        switch(state){
            case State::Start: 
                get_nbc(C,source_file,forward);
                break;
            default:
                break;
        }
    }while(1);
    return 0;
}