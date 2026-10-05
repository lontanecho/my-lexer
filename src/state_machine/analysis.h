#pragma once
#include <string>
#include <vector>
#include "data_structure/translate_table.h"

struct LexError {
    int line;
    int column;
    std::string message;
};

struct LexerStats {
    size_t lineCount = 0;
    size_t codeLineCount = 0;
    size_t characterCount = 0;
    size_t commentCount = 0;
    size_t tokenCount = 0;
    size_t keywordCount = 0;
    size_t identifierCount = 0;
    size_t intCount = 0;
    size_t floatCount = 0;
    size_t charCount = 0;
    size_t stringCount = 0;
    size_t operatorCount = 0;
    size_t delimiterCount = 0;
    size_t preprocessorCount = 0;
};

struct LexResult {
    LexerStats stats;
    std::vector<LexError> errors;
};

LexResult AnalyzeSource(const std::string& src, TranslateTable& transtable);
