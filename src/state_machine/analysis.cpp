#include "state_machine/analysis.h"
#include "data_structure/keyword_table.h"
#include "utils/tools.h"
#include <string_view>
#include <unordered_map>

namespace {
struct OperatorSpec {
    std::string_view lexeme;
    TokenType type;
};

bool identifierStart(char c) { return letter(c) || c == '_'; }
bool identifierPart(char c) { return identifierStart(c) || digit(c); }
bool horizontalSpace(char c) { return c == ' ' || c == '\t' || c == '\f' || c == '\v'; }
bool newline(char c) { return c == '\n' || c == '\r'; }
bool simpleEscape(char c) {
    return c == '\\' || c == '\'' || c == '"' || c == '?' || c == 'a' ||
        c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'v' || c == '0';
}

// 所有前进操作集中于 advance；位置和统计随扫描同步更新。
class Scanner {
public:
    Scanner(const std::string& source, TranslateTable& table) : src(source), tokens(table) {}
    LexResult run();
private:
    const std::string& src;
    TranslateTable& tokens;
    KeywordTable keywords;
    LexResult result;
    size_t index = 0;
    int line = 1, column = 1;
    int lastCodeLine = 0;
    bool lineHasToken = false;

    char peek(size_t offset = 0) const {
        return offset < src.size() - index ? src[index + offset] : '\0';
    }
    void advance() {
        const char c = src[index++];
        ++result.stats.characterCount;
        if (newline(c)) {
            if (c != '\n' || index < 2 || src[index - 2] != '\r') ++line;
            column = 1;
            lineHasToken = false;
        } else ++column;
        // 文件尾的换行不额外增加一个空行；空文件为 0 行。
        result.stats.lineCount = static_cast<size_t>(line - (newline(c) ? 1 : 0));
    }
    void error(Pos pos, const std::string& message) {
        result.errors.push_back({pos.line, pos.column, message});
    }
    void emit(TokenType type, size_t start, Pos pos) {
        tokens.add(type, src.substr(start, index - start), pos.line, pos.column);
        if (lastCodeLine != pos.line) {
            ++result.stats.codeLineCount;
            lastCodeLine = pos.line;
        }
        ++result.stats.tokenCount;
        const int value = static_cast<int>(type);
        if (value >= 1 && value <= static_cast<int>(TokenType::KwSizeof)) ++result.stats.keywordCount;
        else if (type == TokenType::Identifier) ++result.stats.identifierCount;
        else if (type == TokenType::IntLiteral) ++result.stats.intCount;
        else if (type == TokenType::FloatLiteral) ++result.stats.floatCount;
        else if (type == TokenType::CharLiteral) ++result.stats.charCount;
        else if (type == TokenType::StringLiteral) ++result.stats.stringCount;
        else if (value >= 201 && value < 301) ++result.stats.operatorCount;
        else if (value >= 301 && value < 401) ++result.stats.delimiterCount;
        else if (value >= 401) ++result.stats.preprocessorCount;
    }
};

LexResult Scanner::run() {
    static constexpr OperatorSpec operators[] = {
        {"->*", TokenType::ArrowStar},
        {"<<=", TokenType::ShlAssign}, {">>=", TokenType::ShrAssign},
        {"++", TokenType::PlusPlus}, {"--", TokenType::MinusMinus},
        {"+=", TokenType::PlusAssign}, {"-=", TokenType::MinusAssign},
        {"*=", TokenType::StarAssign}, {"/=", TokenType::SlashAssign},
        {"%=", TokenType::PercentAssign}, {"==", TokenType::Eq},
        {"<=", TokenType::Le}, {">=", TokenType::Ge},
        {"&&", TokenType::And}, {"||", TokenType::Or},
        {"&=", TokenType::AndAssign}, {"|=", TokenType::OrAssign},
        {"^=", TokenType::XorAssign}, {"->", TokenType::Arrow},
        {"<<", TokenType::Shl}, {">>", TokenType::Shr},
        {"!=", TokenType::Ne}, {".*", TokenType::DotStar},
        {"::", TokenType::Scope},
        {"+", TokenType::Plus}, {"-", TokenType::Minus},
        {"*", TokenType::Star}, {"/", TokenType::Slash},
        {"%", TokenType::Percent}, {"=", TokenType::Assign},
        {"<", TokenType::Lt}, {">", TokenType::Gt},
        {"!", TokenType::Not}, {"&", TokenType::BitAnd},
        {"|", TokenType::BitOr}, {"^", TokenType::BitXor},
        {"~", TokenType::BitNot}, {"?", TokenType::Question},
        {".", TokenType::Dot}, {":", TokenType::Colon},
        {"(", TokenType::LParen}, {")", TokenType::RParen},
        {"{", TokenType::LBrace}, {"}", TokenType::RBrace},
        {"[", TokenType::LBracket}, {"]", TokenType::RBracket},
        {";", TokenType::Semicolon}, {",", TokenType::Comma}
    };
    static const std::unordered_map<std::string, TokenType> directives = {
        {"#include",TokenType::PpInclude},{"#define",TokenType::PpDefine},{"#undef",TokenType::PpUndef},
        {"#if",TokenType::PpIf},{"#ifdef",TokenType::PpIfdef},{"#ifndef",TokenType::PpIfndef},
        {"#else",TokenType::PpElse},{"#elif",TokenType::PpElif},{"#endif",TokenType::PpEndif},
        {"#pragma",TokenType::PpPragma},{"#error",TokenType::PpError},{"#warning",TokenType::PpWarning},
        {"#line",TokenType::PpLine}
    };

    while (index < src.size()) {
        const char c = peek();
        if (horizontalSpace(c) || newline(c)) { advance(); continue; }
        const size_t start = index;
        const Pos pos{line, column};
        if (c == '/' && (peek(1) == '/' || peek(1) == '*')) {
            const bool block = peek(1) == '*';
            ++result.stats.commentCount;
            advance(); advance();
            if (!block) {
                while (index < src.size() && !newline(peek())) advance();
            } else {
                while (index < src.size() && !(peek() == '*' && peek(1) == '/')) advance();
                if (index == src.size()) error(pos, "块注释未闭合");
                else { advance(); advance(); }
            }
            continue;
        }
        const bool directiveAllowed = !lineHasToken;
        lineHasToken = true;
        if (c == '#' && directiveAllowed) {
            advance();
            while (index < src.size() && horizontalSpace(peek())) advance();
            const size_t nameStart = index;
            while (index < src.size() && identifierPart(peek())) advance();
            const auto it = directives.find("#" + src.substr(nameStart, index - nameStart));
            if (it == directives.end()) error(pos, "未知的预处理指令");
            else emit(it->second, start, pos);
            continue;
        }
        if (identifierStart(c)) {
            do { advance(); } while (index < src.size() && identifierPart(peek()));
            const TokenType type = keywords.find(src.substr(start, index - start));
            emit(type == TokenType::Error ? TokenType::Identifier : type, start, pos);
            continue;
        }
        if (digit(c) || (c == '.' && digit(peek(1)))) {
            bool floating = false, bad = false;
            while (index < src.size() && digit(peek())) advance();
            if (peek() == '.') {
                floating = true; advance();
                while (index < src.size() && digit(peek())) advance();
            }
            if (peek() == 'e' || peek() == 'E') {
                floating = true; advance();
                if (peek() == '+' || peek() == '-') advance();
                const size_t exponentStart = index;
                while (index < src.size() && digit(peek())) advance();
                bad = exponentStart == index;
            }
            // 将 123abc、1.2.3 等作为一个错误词素，保留分隔符继续扫描。
            if (identifierStart(peek()) || peek() == '.') {
                bad = true;
                while (index < src.size() && (identifierPart(peek()) || peek() == '.')) advance();
            }
            if (bad) error(pos, "非法数字常量（数字格式或指数部分错误）");
            else emit(floating ? TokenType::FloatLiteral : TokenType::IntLiteral, start, pos);
            continue;
        }
        if (c == '\'' || c == '"') {
            const char quote = c;
            advance();
            bool closed = false, bad = false;
            size_t characterUnits = 0;
            while (index < src.size() && !newline(peek())) {
                if (peek() == quote) { advance(); closed = true; break; }
                const Pos contentPos{line, column};
                if (peek() == '\\') {
                    advance();
                    if (index == src.size() || newline(peek())) break;
                    if (!simpleEscape(peek())) {
                        error(contentPos, "常量中的转义序列非法");
                        bad = true;
                    }
                    advance();
                } else {
                    if (peek() == '\0') {
                        error(contentPos, "常量中包含非法空字节");
                        bad = true;
                    }
                    advance();
                }
                ++characterUnits;
            }
            if (!closed) {
                error(pos, quote == '\'' ? "字符常量未闭合" : "字符串常量未闭合");
                bad = true;
            } else if (quote == '\'' && characterUnits != 1) {
                error(pos, "字符常量必须只包含一个字符");
                bad = true;
            }
            if (!bad) emit(quote == '\'' ? TokenType::CharLiteral : TokenType::StringLiteral, start, pos);
            continue;
        }
        bool matched = false;
        for (const OperatorSpec& spec : operators) {
            if (src.compare(index, spec.lexeme.size(), spec.lexeme) == 0) {
                for (size_t n = 0; n < spec.lexeme.size(); ++n) advance();
                emit(spec.type, start, pos);
                matched = true;
                break;
            }
        }
        if (!matched) {
            error(pos, "非法字符");
            advance();
        }
    }
    tokens.add(TokenType::Eof, "", line, column);
    return result;
}
} // namespace

LexResult AnalyzeSource(const std::string& src, TranslateTable& transtable) {
    return Scanner(src, transtable).run();
}
