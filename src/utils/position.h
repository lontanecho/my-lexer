#pragma once
#include<string>
#include<vector>

struct Pos{
    int line;
    int column;
};

inline std::vector<Pos> buildPositions(const std::string& src) {
    std::vector<Pos> positions;
    positions.resize(src.size() + 1);
    int line = 1, col = 1;
    for (size_t i = 0; i < src.size(); ++i) {
        positions[i] = {line, col};
        if (src[i] == '\r') {
            // Treat CRLF as one logical newline.
            ++line;
            col = 1;
        } else if (src[i] == '\n') {
            if (i == 0 || src[i - 1] != '\r') {
                ++line;
                col = 1;
            }
        } else {
            ++col;
        }
    }
    positions[src.size()] = {line, col};
    return positions;
}
