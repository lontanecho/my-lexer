#pragma once
#include<string>
#include<vector>

struct Pos{
    int line;
    int column;
};

std::vector<Pos> buildPositions(const std::string& src) {
    std::vector<Pos> positions;
    positions.resize(src.size() + 1);
    int line = 1, col = 1;
    for (size_t i = 0; i < src.size(); ++i) {
        positions[i] = {line, col};
        if (src[i] == '\n') {
            ++line;
            col = 1;
        } else if (src[i] == '\r') {
            if (i + 1 < src.size() && src[i+1] == '\n') {
                // 
            }
            ++line;
            col = 1;
        } else {
            ++col;
        }
    }
    positions[src.size()] = {line, col};
    return positions;
}