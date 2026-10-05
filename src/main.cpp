#include <fstream>
#include <iostream>
#include <sstream>
#include "state_machine/analysis.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <文件名>" << std::endl;
        return 1;
    }
    // 二进制读取，避免 Windows 自动转换 CRLF 或将 Ctrl-Z 当作文件结束。
    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    if (file.bad()) {
        std::cerr << "读取文件失败: " << argv[1] << std::endl;
        return 1;
    }

    TranslateTable table;
    const LexResult result = AnalyzeSource(buffer.str(), table);
    table.print();
    for (const LexError& error : result.errors) {
        std::cerr << "词法错误: 第" << error.line << "行 第" << error.column
            << "列，" << error.message << std::endl;
    }
    const LexerStats& stats = result.stats;
    std::cout << "\n统计结果:\n"
        << "源程序行数: " << stats.lineCount << '\n'
        << "代码行数: " << stats.codeLineCount << '\n'
        << "字符总数（字节，含空白和注释）: " << stats.characterCount << '\n'
        << "注释个数: " << stats.commentCount << '\n'
        << "关键字: " << stats.keywordCount << '\n'
        << "标识符: " << stats.identifierCount << '\n'
        << "整数常量: " << stats.intCount << '\n'
        << "浮点常量: " << stats.floatCount << '\n'
        << "字符常量: " << stats.charCount << '\n'
        << "字符串常量: " << stats.stringCount << '\n'
        << "运算符: " << stats.operatorCount << '\n'
        << "界符: " << stats.delimiterCount << '\n'
        << "预处理指令: " << stats.preprocessorCount << '\n'
        << "有效单词总数(不含 EOF): " << stats.tokenCount << '\n'
        << "词法错误数: " << result.errors.size() << std::endl;
    return result.errors.empty() ? 0 : 1;
}
