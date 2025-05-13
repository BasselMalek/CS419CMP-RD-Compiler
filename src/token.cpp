#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    int line;
    std::string text;
    std::string type;
    bool error;

    Token(int line = 0, const std::string& text = "", const std::string& type = "", bool error = false)
        : line(line), text(text), type(type), error(error) {}
};

#endif // TOKEN_H
