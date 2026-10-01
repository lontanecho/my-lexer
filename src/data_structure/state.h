#pragma once
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