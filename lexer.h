#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    std::unordered_map<std::string, std::string> keywords;
    std::unordered_set<std::string> includedFiles;

    char peek(int n = 0);
    char get();
    void skipWhitespace();
    TokenType TokenTypeFromString(const std::string& typeStr);
    Token lexIdentifierOrKeyword();
    Token lexNumber();
    Token lexString();
    Token lexChar();
    Token lexOperatorOrPunctuation();
    std::vector<Token> lexComment();
};

#endif
