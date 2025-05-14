#pragma once
#include <string>

enum TokenType {
  CLEAR,
  CONDITION,
  INTEGER,
  SINTEGER,
  CHARACTER,
  STRING,
  FLOAT,
  SFLOAT,
  VOID,
  LOOP,
  RETURN,
  ARITHMETIC_OP,
  LOGIC_OP,
  RELATIONAL_OP,
  ASSIGNMENT_OP,
  ACCESS_OP,
  BRACE,
  CONSTANT,
  QUOTATION_MARK,
  INCLUSION,
  STRUCT,
  BREAK,
  IDENTIFIER,
  COMMA,
  COLON,
  SEMICOLON,
  COMMENT_START,
  COMMENT_CONTENT,
  COMMENT_END,
  SINGLE_LINE_COMMENT_START,
  SINGLE_LINE_COMMENT_CONTENT,
  INVALID,
  INVALID_COMMENT,
  INVALID_INCLUSION,
  STRING_LITERAL,
  CHARACTER_LITERAL,
  UNTERMINATED_STRING,
  UNTERMINATED_CHAR,
  INVALID_IDENTIFIER,
  ADDOP,
  MULOP,
  AMPERSAND,
  UNKNOWN,
  EOF_TOKEN
};

std::string tokenTypeToString(TokenType type);

struct Token {
  TokenType type;
  int line;
  bool error;
  std::string text;
  Token() = default;
  Token(int line, TokenType t, std::string val, bool error=false);
};