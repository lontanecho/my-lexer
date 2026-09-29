#include<string>
#include<unordered_map>


class KeywordTable{
public:
    KeywordTable();
    int find(const std::string &lexeme)const;
private:
    const std::unordered_map<std::string ,int> table;
};
