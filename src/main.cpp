#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include"data_structure/keyword_table.h"
#include"data_structure/symbol_table.h" 
#include"utils/tools.h"

int state;          
char C;
int iskey;
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
    state=0;
    lexemebegin = source_file.data();
    forward = lexemebegin;
    KeywordTable keytable;
    SymbolTable symtable;

    do{
        switch(state){
            case 0:
                get_char(C,source_file,forward);
                if(letter(C)){
                    state=1;
                    cat(C,token);
                }
                else if(digit(C)){
                    state=2;
                    cat(C,token);
                }
                else if(C=='.'){
                    state=3;
                    cat(C,token);
                }
                else if(C=='"'){
                    state=4;
                }
            }
    }while(1);
    return 0;
}