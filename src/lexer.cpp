#include "Lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip>
#include "helpers.h"

using namespace std;

Lexer::Lexer(const string& sourceCode)
    : source(sourceCode), pos(0), line(1) {
    keywords = {
        {"IfTrue", "Condition"},
        {"Imw", "Integer"},
        {"SIMw", "SInteger"},
        {"Chj", "Character"},
        {"Series", "String"},
        {"IMwf", "Float"},
        {"SIMwf", "SFloat"},
        {"NOReturn", "Void"},
        {"Reiterate", "Loop"},
        {"Turnback", "Return"},
        {"OutLoop", "Break"},
        {"Loli", "Struct"},
        {"include", "Inclusion"},
        {"int", "Type"},
        {"Stop", "Break"},
        {"Otherwise", "Condition"}
    };
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (pos < source.size()) {
        skipWhitespace();
        if (pos >= source.size()) break;

        char current = peek();

        if (current == '/' && peek(1) == '/') {
            Token token;
            token.line = line;
            token.type = TokenType::SINGLE_LINE_COMMENT_START;
            token.text = "//";
            get(); get();
            tokens.push_back(token);

            Token contentToken;
            contentToken.line = line;
            contentToken.type = TokenType::SINGLE_LINE_COMMENT_CONTENT;
            string content;
            while (pos < source.size() && peek() != '\n') {
                content.push_back(get());
            }
            contentToken.text = content;
            tokens.push_back(contentToken);
            continue;
        } else if (current == '/' && peek(1) == '*') {
            Token token;
            token.line = line;
            token.type = TokenType::COMMENT_START;
            token.text = "/*";
            get(); get();
            tokens.push_back(token);

            Token contentToken;
            contentToken.line = line;
            contentToken.type = TokenType::COMMENT_CONTENT;
            string content;
            while (pos < source.size() && !(peek() == '*' && peek(1) == '/')) {
                content.push_back(get());
            }
            contentToken.text = content;
            tokens.push_back(contentToken);

            Token endToken;
            endToken.line = line;
            if (peek() == '*' && peek(1) == '/') {
                endToken.type = TokenType::COMMENT_END;
                endToken.text = "*/";
                get(); get();
            } else {
                endToken.type = TokenType::INVALID_COMMENT;
                endToken.text = "";
                endToken.error = true;
            }
            tokens.push_back(endToken);
            continue;
        } else if (current == '/' && (peek(1) == '@' || peek(1) == '^')) {
            vector<Token> commentTokens = lexComment();
            tokens.insert(tokens.end(), commentTokens.begin(), commentTokens.end());
            continue;
        } else if (current == '"') {
            tokens.push_back(lexString());
            continue;
        } else if (current == '\'') {
            tokens.push_back(lexChar());
            continue;
        } else if (isalpha(current) || current == '_') {
            Token keywordToken = lexIdentifierOrKeyword();

            if (keywordToken.type == TokenType::INCLUSION) {
                skipWhitespace();
                if (peek() == '"') {
                    Token fileToken = lexString();
                    string includedFile = fileToken.text;
                    if (includedFile.size() >= 2) {
                        includedFile = includedFile.substr(1, includedFile.size() - 2);
                    }

                    if (includedFiles.find(includedFile) != includedFiles.end()) {
                        Token errorToken;
                        errorToken.line = line;
                        errorToken.text = includedFile;
                        errorToken.type = TokenType::INVALID_INCLUSION;
                        errorToken.error = true;
                        tokens.push_back(errorToken);
                        continue;
                    }

                    std::ifstream fileCheck(includedFile);
                    if (!fileCheck.is_open()) {
                        Token errorToken;
                        errorToken.line = line;
                        errorToken.text = includedFile;
                        errorToken.type = TokenType::INVALID_INCLUSION;
                        errorToken.error = true;
                        tokens.push_back(keywordToken);
                        tokens.push_back(errorToken);
                        continue;
                    }
                    fileCheck.close();

                    string includedCode = readFile(includedFile);
                    if (includedCode.empty()) {
                        Token errorToken;
                        errorToken.line = line;
                        errorToken.text = includedFile;
                        errorToken.type = TokenType::INVALID_INCLUSION;
                        errorToken.error = true;
                        tokens.push_back(keywordToken);
                        tokens.push_back(errorToken);
                    } else {
                        includedFiles.insert(includedFile);
                        Lexer includedLexer(includedCode);
                        vector<Token> includedTokens = includedLexer.tokenize();
                        tokens.insert(tokens.end(), includedTokens.begin(), includedTokens.end());
                        tokens.push_back(keywordToken);
                        tokens.push_back(fileToken);
                    }
                } else {
                    keywordToken.error = true;
                    keywordToken.type = TokenType::INVALID_INCLUSION;
                    tokens.push_back(keywordToken);
                }
            } else {
                tokens.push_back(keywordToken);
            }
            continue;
        } else if (isdigit(current) || (current == '-' && isdigit(peek(1))) || (current == '+' && isdigit(peek(1)))) {
            tokens.push_back(lexNumber());
            continue;
        } else {
            tokens.push_back(lexOperatorOrPunctuation());
            continue;
        }
    }

    Token eofToken;
    eofToken.line = line;
    eofToken.text = "";
    eofToken.type = TokenType::EOF_TOKEN;
    eofToken.error = false;
    tokens.push_back(eofToken);

    return tokens;
}

char Lexer::peek(int n) {
    if (n < 0 || pos + n >= source.size())
        return '\0';
    return source[pos + n];
}

char Lexer::get() {
    char c = source[pos++];
    if (c == '\n')
        line++;
    return c;
}

void Lexer::skipWhitespace() {
    while (pos < source.size() && isspace(source[pos])) {
        if (source[pos] == '\n')
            line++;
        pos++;
    }
}

TokenType Lexer::TokenTypeFromString(const string& typeStr) {
    if (typeStr == "Condition") return TokenType::CONDITION;
    if (typeStr == "Integer") return TokenType::INTEGER;
    if (typeStr == "SInteger") return TokenType::SINTEGER;
    if (typeStr == "Character") return TokenType::CHARACTER;
    if (typeStr == "String") return TokenType::STRING;
    if (typeStr == "Float") return TokenType::FLOAT;
    if (typeStr == "SFloat") return TokenType::SFLOAT;
    if (typeStr == "Void") return TokenType::VOID;
    if (typeStr == "Loop") return TokenType::LOOP;
    if (typeStr == "Return") return TokenType::RETURN;
    if (typeStr == "Break") return TokenType::BREAK;
    if (typeStr == "Struct") return TokenType::STRUCT;
    if (typeStr == "Inclusion") return TokenType::INCLUSION;
    if (typeStr == "Type") return TokenType::INTEGER;
    return TokenType::INVALID;
}

Token Lexer::lexIdentifierOrKeyword() {
    Token token;
    token.line = line;
    token.error = false;
    string result;

    while (pos < source.size() && (isalnum(peek()) || peek() == '_')) {
        result.push_back(get());
    }

    if (result == "Repeat") {
        skipWhitespace();
        string nextWord;
        size_t tempPos = pos;
        while (tempPos < source.size() && (isalnum(source[tempPos]) || source[tempPos] == '_')) {
            nextWord.push_back(source[tempPos++]);
        }
        if (nextWord == "When") {
            pos = tempPos;
            result += " " + nextWord;
            token.type = TokenType::LOOP;
            token.text = result;
            return token;
        }
    }

    if (keywords.find(result) != keywords.end()) {
        token.type = TokenTypeFromString(keywords[result]);
    } else {
        token.type = TokenType::IDENTIFIER;
    }
    token.text = result;
    return token;
}

Token Lexer::lexNumber() {
    Token token;
    token.line = line;
    token.error = false;
    string numberStr;
    bool seenDot = false;

    if (peek() == '-' || peek() == '+') {
        numberStr.push_back(get());
    }

    while (pos < source.size() && (isdigit(peek()) || peek() == '.')) {
        char c = peek();
        if (c == '.') {
            if (seenDot) {
                token.error = true;
            }
            seenDot = true;
        }
        numberStr.push_back(get());
    }

    if (pos < source.size() && (isalpha(peek()) || peek() == '_')) {
        token.error = true;
        while (pos < source.size() && (isalnum(peek()) || peek() == '_')) {
            numberStr.push_back(get());
        }
        token.type = TokenType::INVALID_IDENTIFIER;
    } else {
      // token.type = seenDot ? TokenType::FLOAT : TokenType::INTEGER;
       token.type = CONSTANT;
    }

    token.text = numberStr;
    return token;
}

Token Lexer::lexString() {
    Token token;
    token.line = line;
    token.error = false;
    string str = "\"";
    get();

    while (pos < source.size() && peek() != '"') {
        str.push_back(get());
    }

    if (peek() == '"') {
        str.push_back(get());
        token.type = TokenType::STRING_LITERAL;
    } else {
        token.type = TokenType::UNTERMINATED_STRING;
        token.error = true;
    }

    token.text = str;
    return token;
}

Token Lexer::lexChar() {
    Token token;
    token.line = line;
    token.error = false;
    string str = "'";
    get();

    if (pos < source.size() && peek() != '\'') {
        str.push_back(get());
    }

    if (peek() == '\'') {
        str.push_back(get());
        token.type = TokenType::CHARACTER_LITERAL;
    } else {
        token.type = TokenType::UNTERMINATED_CHAR;
        token.error = true;
    }

    token.text = str;
    return token;
}

Token Lexer::lexOperatorOrPunctuation() {
    Token token;
    token.line = line;
    token.error = false;
    char current = get();
    string op(1, current);

    switch (current) {
        case '=':
            if (peek() == '=') { op += get(); token.type = TokenType::RELATIONAL_OP; }
            else token.type = TokenType::ASSIGNMENT_OP;
            break;
        case '<':
        case '>':
            if (peek() == '=') { op += get(); }
            token.type = TokenType::RELATIONAL_OP;
            break;
        case '!':
            if (peek() == '=') { op += get(); token.type = TokenType::RELATIONAL_OP; }
            else token.type = TokenType::LOGIC_OP;
            break;
        case '&':
            if (peek() == '&') { op += get(); token.type = TokenType::LOGIC_OP; }
            else token.type = TokenType::AMPERSAND;
            break;
        case '|':
            if (peek() == '|') { op += get(); token.type = TokenType::LOGIC_OP; }
            else token.type = TokenType::ARITHMETIC_OP;
            break;
        case '-':
            if (peek() == '>') { op += get(); token.type = TokenType::ACCESS_OP; }
            else token.type = TokenType::ADDOP;
            break;
        case '+': token.type = TokenType::ADDOP; break;
        case '*': token.type = TokenType::MULOP; break;
        case '/': token.type = TokenType::MULOP; break;
        case '~': token.type = TokenType::LOGIC_OP; break;
        case '(': case ')': case '{': case '}': case '[': case ']':
            token.type = TokenType::BRACE; break;
        case ';': token.type = TokenType::SEMICOLON; break;
        case ',': token.type = TokenType::COMMA; break;
        default:
            token.type = TokenType::UNKNOWN;
            token.error = true;
            break;
    }

    token.text = op;
    return token;
}

vector<Token> Lexer::lexComment() {
    vector<Token> commentTokens;
    Token tokenStart;
    tokenStart.line = line;
    tokenStart.error = false;

    char second = peek(1);
    if (second == '@') {
        tokenStart.text = "/@";
        tokenStart.type = TokenType::COMMENT_START;
        get(); get();
        commentTokens.push_back(tokenStart);

        Token tokenContent;
        tokenContent.line = line;
        tokenContent.error = false;
        string content;
        while (pos < source.size() && !(peek() == '@' && peek(1) == '/')) {
            content.push_back(get());
        }
        tokenContent.text = content;
        tokenContent.type = TokenType::COMMENT_CONTENT;
        commentTokens.push_back(tokenContent);

        Token tokenEnd;
        tokenEnd.line = line;
        tokenEnd.error = false;
        if (peek() == '@' && peek(1) == '/') {
            tokenEnd.text = "@/";
            tokenEnd.type = TokenType::COMMENT_END;
            get(); get();
        } else {
            tokenEnd.text = "";
            tokenEnd.type = TokenType::INVALID_COMMENT;
            tokenEnd.error = true;
        }
        commentTokens.push_back(tokenEnd);
    } else if (second == '^') {
        tokenStart.text = "/^";
        tokenStart.type = TokenType::SINGLE_LINE_COMMENT_START;
        get(); get();
        commentTokens.push_back(tokenStart);

        Token tokenContent;
        tokenContent.line = line;
        tokenContent.error = false;
        string content;
        while (pos < source.size() && peek() != '\n') {
            content.push_back(get());
        }
        tokenContent.text = content;
        tokenContent.type = TokenType::SINGLE_LINE_COMMENT_CONTENT;
        commentTokens.push_back(tokenContent);
    }
    return commentTokens;
}
