#pragma once

#include <string>
#include <vector>

// Token type enumeration
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
  SEMICOLON
};

// Token structure
struct Token {
  TokenType type;
  std::string value;
  Token() = default;
  Token(TokenType t, std::string val);
};

// Parser class declaration
class Parser {
private:
  std::vector<Token> tokens;
  Token current_token;
  unsigned int token_index;
  unsigned int line_count;
  unsigned int error_count;

  bool isDataType(TokenType token);
  bool isStartOfStatement(TokenType type);
  bool isStartsOfLine(TokenType token);
  void nextToken();
  void throwError();

  // Declaration parsing methods
  void parseDeclarations();
  void parseDeclarationList();
  void parseDeclaration();
  void parseVarDec();
  void parseTypeSpecifier();
  void parseFunDec();
  void parseParams();
  void parseParamList();
  void parsePList();
  void parseParam();

  // Statement parsing methods
  void parseCompoundStmt();
  void parseLocalDecs();
  void parseStmtList();
  void parseStatement();
  void parseExpressionStmt();
  void parseSelectionStmt();
  void parseIterationStmt();
  void parseJumpStmt();

  // Expression parsing methods
  void parseExpression();
  void parseIdAssign();
  void parseSimpleExpression();
  void parseRelop();
  void parseAdditiveExpression();
  void parseAdditiveExpressionPrime();
  void parseAddOp();
  void parseTerm();
  void parseMulOp();
  void parseTermPrime();
  void parseFactor();
  void parseCall();
  void parseArgs();
  void parseArgList();
  void parseAList();

  // Number parsing methods
  void parseNum();
  void parseUnsignedNum();
  void parseSignedNum();
  void parsePosNum();
  void parseNegNum();
  void parseValue();

  // Other utility methods
  void parseComment();
  void parseIncludeCommand();
  void parseFName();

public:
  Parser();
  void setTokens(const std::vector<Token> &input_tokens);
  int parse();
  unsigned int getErrorCount() const;
};
