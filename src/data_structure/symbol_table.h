#include<string>
#include<unordered_map>

class SymbolTable{
public:
    SymbolTable();
    int find(const std::string &lexeme)const;
    void add(const std::string &lexeme);

private:
    mutable int id;
    std::unordered_map<std::string, int> table;
};