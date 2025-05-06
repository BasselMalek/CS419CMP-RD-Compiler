#include "parser.h"
#include <iostream>

Token::Token(TokenType t, std::string val) {
  this->type = t;
  this->value = val;
}

Parser::Parser() : token_index(0), line_count(0), error_count(0) {}

void Parser::setTokens(const std::vector<Token> &input_tokens) {
  tokens = input_tokens;
  token_index = 0;
  if (!tokens.empty()) {
    current_token = tokens[0];
  }
}

int Parser::parse() {
  if (tokens.empty()) {
    std::cout << "Error: No tokens to parse." << std::endl;
    return 1;
  }

  if (current_token.type == CLEAR) {
    nextToken();
    parseDeclarations();

    if (error_count == 0) {
      std::cout << "Parsing completed successfully." << std::endl;
      return 0;
    } else {
      std::cout << "Parsing completed with " << error_count << " errors."
                << std::endl;
      return 1;
    }
  } else {
    std::cout << "Invalid token stream. Rescan and try again." << std::endl;
    return 1;
  }
}

unsigned int Parser::getErrorCount() const { return error_count; }

bool Parser::isDataType(TokenType token) {
  switch (token) {
  case INTEGER:
  case SINTEGER:
  case CHARACTER:
  case STRING:
  case FLOAT:
  case SFLOAT:
  case VOID:
    return true;
  default:
    return false;
  }
}

bool Parser::isStartOfStatement(TokenType type) {
  return type == IDENTIFIER || type == CONSTANT ||
         (type == BRACE && (current_token.value == "(") ||
          (current_token.value == "{")) ||
         type == CONDITION || type == LOOP || type == RETURN || type == BREAK;
}

bool Parser::isStartsOfLine(TokenType token) {
  switch (token) {
  case CLEAR:
  case CONDITION:
  case LOOP:
  case RETURN:
  case BREAK:
  case STRUCT:
  case IDENTIFIER:
  case INCLUSION:
  case VOID:
  case INTEGER:
  case SINTEGER:
  case CHARACTER:
  case STRING:
  case FLOAT:
  case SFLOAT:
    return true;
  default:
    return false;
  }
}

void Parser::nextToken() {
  if (token_index + 1 < tokens.size()) {
    current_token = tokens[++token_index];
    line_count += (isStartsOfLine(current_token.type)) ? 1 : 0;
  } else {
    current_token = Token(CLEAR, "EOF");
  }
}

void Parser::throwError() {
  error_count++;
  std::cout << "Error at line " << line_count << ": Unexpected token "
            << current_token.value << std::endl;

  while (current_token.type != SEMICOLON && token_index + 1 < tokens.size()) {
    nextToken();
  }
  if (current_token.type == SEMICOLON && token_index + 1 < tokens.size()) {
    nextToken();
    if (isDataType(current_token.type)) {
      parseDeclaration();
    } else if (token_index + 1 < tokens.size()) {
      parseStatement();
    }
  }
}

void Parser::parseDeclarations() {
  parseDeclaration();
  parseDeclarationList();
}

void Parser::parseDeclarationList() {
  if (isDataType(current_token.type)) {
    parseDeclaration();
    parseDeclarationList();
  }
}

void Parser::parseDeclaration() {
  if (isDataType(current_token.type)) {
    parseTypeSpecifier();
    if (current_token.type == IDENTIFIER) {
      nextToken();
      if (current_token.type == BRACE && current_token.value == "(") {
        parseFunDec();
      } else {
        parseVarDec();
      }
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseVarDec() {
  if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  } else if (current_token.type == COMMA) {
    nextToken();
    if (current_token.type == IDENTIFIER) {
      nextToken();
      parseVarDec();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseTypeSpecifier() {
  if (isDataType(current_token.type)) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseFunDec() {
  if (current_token.type == BRACE && current_token.value == "(") {
    nextToken();
    parseParams();
    if (current_token.type == BRACE && current_token.value == ")") {
      nextToken();
      if (current_token.type == BRACE && current_token.value == "{") {
        parseCompoundStmt();
      } else {
        throwError();
      }
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseParams() {
  if (current_token.type == VOID) {
    nextToken();
  } else if (isDataType(current_token.type)) {
    parseParamList();
  }

  if (current_token.type == BRACE && current_token.value == ")") {
    return;
  } else {
    throwError();
  }
}

void Parser::parseParamList() {
  parseParam();
  parsePList();
}

void Parser::parsePList() {
  if (current_token.type == BRACE && current_token.value == ")") {
    return;
  } else if (current_token.type == COMMA) {
    nextToken();
    parseParam();
    parsePList();
  } else {
    throwError();
  }
}

void Parser::parseParam() {
  if (isDataType(current_token.type)) {
    nextToken();
  } else {
    throwError();
    return;
  }

  if (current_token.type == IDENTIFIER) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseCompoundStmt() {
  if (current_token.type == BRACE && current_token.value == "{") {
    nextToken();
    parseLocalDecs();
    parseStmtList();
    if (current_token.type == BRACE && current_token.value == "}") {
      nextToken();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseLocalDecs() {
  if (isDataType(current_token.type)) {
    parseVarDec();
    parseLocalDecs();
  }
}

void Parser::parseStmtList() {
  if (isStartOfStatement(current_token.type)) {
    parseStatement();
    parseStmtList();
  }
}

void Parser::parseStatement() {
  switch (current_token.type) {
  case IDENTIFIER:
  case CONSTANT:
    parseExpressionStmt();
    break;
  case BRACE:
    if (current_token.value == "(") {
      parseExpressionStmt();
    } else if (current_token.value == "{") {
      parseCompoundStmt();
    } else {
      throwError();
    }
    break;
  case CONDITION:
    parseSelectionStmt();
    break;
  case LOOP:
    parseIterationStmt();
    break;
  case RETURN:
  case BREAK:
    parseJumpStmt();
    break;
  default:
    throwError();
    while (current_token.type != SEMICOLON &&
           !(current_token.type == BRACE && current_token.value == "}") &&
           token_index + 1 < tokens.size()) {
      nextToken();
    }
  }
}

void Parser::parseExpressionStmt() {
  if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  }

  parseExpression();

  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseSelectionStmt() {
  if (current_token.type == CONDITION) {
    nextToken();
    if (current_token.type == BRACE && current_token.value == "(") {
      nextToken();
      parseExpression();
      if (current_token.type == BRACE && current_token.value == ")") {
        nextToken();
        parseStatement();
        if (current_token.value == "else") {
          nextToken();
          parseStatement();
        }
      } else {
        throwError();
      }
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseIterationStmt() {
  if (current_token.type == LOOP) {
    nextToken();
    if (current_token.value == "RepeatWhen") {
      if (current_token.type == BRACE && current_token.value == "(") {
        nextToken();
        parseExpression();
        if (current_token.type == BRACE && current_token.value == ")") {
          nextToken();
          parseStatement();
        } else {
          throwError();
        }
      } else {
        throwError();
      }
    } else if (current_token.value == "Reiterate") {
      if (current_token.type == BRACE && current_token.value == "(") {
        nextToken();
        parseExpression();
        if (current_token.type == SEMICOLON) {
          nextToken();
          parseExpression();
          if (current_token.type == SEMICOLON) {
            nextToken();
            parseExpression();
            if (current_token.type == BRACE && current_token.value == ")") {
              nextToken();
              parseStatement();
            } else {
              throwError();
            }
          } else {
            throwError();
          }
        } else {
          throwError();
        }
      } else {
        throwError();
      }
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseJumpStmt() {
  if (current_token.type == RETURN) {
    nextToken();
    if (current_token.type != SEMICOLON) {
      parseExpression();
    }
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError();
    }
  } else if (current_token.type == BREAK) {
    nextToken();
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseExpression() {
  if (current_token.type == IDENTIFIER) {
    parseIdAssign();
    if (current_token.type == ASSIGNMENT_OP) {
      nextToken();
      parseExpression();
    } else {
      parseSimpleExpression();
    }
  } else {
    parseSimpleExpression();
  }
}

void Parser::parseIdAssign() {
  if (current_token.type == IDENTIFIER) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseSimpleExpression() {
  parseAdditiveExpression();
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    parseRelop();
    parseAdditiveExpression();
  }
}

void Parser::parseRelop() {
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseAdditiveExpression() {
  parseTerm();
  parseAdditiveExpressionPrime();
}

void Parser::parseAdditiveExpressionPrime() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    parseAddOp();
    parseTerm();
    parseAdditiveExpressionPrime();
  }
}

void Parser::parseAddOp() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseTerm() {
  parseFactor();
  parseTermPrime();
}

void Parser::parseMulOp() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "*" || current_token.value == "/")) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseTermPrime() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "*" || current_token.value == "/")) {
    parseMulOp();
    parseFactor();
    parseTermPrime();
  }
}

void Parser::parseFactor() {
  switch (current_token.type) {
  case BRACE:
    if (current_token.value == "(") {
      nextToken();
      parseExpression();
      if (current_token.type == BRACE && current_token.value == ")") {
        nextToken();
      } else {
        throwError();
      }
    } else {
      throwError();
    }
    break;
  case IDENTIFIER: {
    std::string id_name = current_token.value;
    nextToken();
    if (current_token.type == BRACE && current_token.value == "(") {
      parseCall();
    }
    // Non-function identifier already consumed
  } break;
  case CONSTANT:
    parseNum();
    break;
  case ARITHMETIC_OP:
    if (current_token.value == "+" || current_token.value == "-") {
      parseSignedNum();
    } else {
      throwError();
    }
    break;
  default:
    throwError();
  }
}

void Parser::parseCall() {
  if (current_token.type == BRACE && current_token.value == "(") {
    nextToken();
    parseArgs();
    if (current_token.type == BRACE && current_token.value == ")") {
      nextToken();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parseArgs() {
  if (!(current_token.type == BRACE && current_token.value == ")")) {
    parseArgList();
  }
}

void Parser::parseArgList() {
  parseExpression();
  parseAList();
}

void Parser::parseAList() {
  if (current_token.type == COMMA) {
    nextToken();
    parseExpression();
    parseAList();
  }
}

void Parser::parseNum() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    parseSignedNum();
  } else if (current_token.type == CONSTANT) {
    parseUnsignedNum();
  } else {
    throwError();
  }
}

void Parser::parseUnsignedNum() { parseValue(); }

void Parser::parseSignedNum() {
  if (current_token.type == ARITHMETIC_OP) {
    if (current_token.value == "+") {
      parsePosNum();
    } else if (current_token.value == "-") {
      parseNegNum();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

void Parser::parsePosNum() {
  if (current_token.type == ARITHMETIC_OP && current_token.value == "+") {
    nextToken();
    parseValue();
  } else {
    throwError();
  }
}

void Parser::parseNegNum() {
  if (current_token.type == ARITHMETIC_OP && current_token.value == "-") {
    nextToken();
    parseValue();
  } else {
    throwError();
  }
}

void Parser::parseValue() {
  if (current_token.type == CONSTANT) {
    nextToken();
  } else {
    throwError();
  }
}

void Parser::parseComment() { nextToken(); }

void Parser::parseIncludeCommand() {
  // Handle include commands if needed
}

void Parser::parseFName() {
  if (current_token.type == STRING) {
    nextToken();
  } else {
    throwError();
  }
}