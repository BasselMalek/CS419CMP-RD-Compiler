#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    std::unordered_map<std::string, std::string> keywords;

    char peek(int n = 0);
    char get();
    void skipWhitespace();

    Token lexIdentifierOrKeyword();
    Token lexNumber();
    Token lexString();
    Token lexChar();
    Token lexOperatorOrPunctuation();
    std::vector<Token> lexComment();
    std::string readFile(const std::string& fileName);
};

#endif // LEXER_H
