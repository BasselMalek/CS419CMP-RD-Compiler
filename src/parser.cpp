#include "parser.h"
#include <iostream>

Parser::Parser()
    : token_index(0), line_count(1), error_count(0), in_function_scope(false) {}

void Parser::setTokens(const std::vector<Token> &input_tokens) {
  tokens = input_tokens;
  token_index = 0;
  if (!tokens.empty()) {
    current_token = tokens[0];
  }
}

int Parser::parse(std::ofstream &out) {
  if (tokens.empty()) {
    std::cout << "Error: No tokens to parse." << std::endl;
    out << "Error: No tokens to parse." << std::endl;
    return 1;
  }
  cout << "\nParser Results:\n";
  cout << string(50, '-') << "\n";
  out << "\nParser Results:\n";
  out << string(50, '-') << "\n";

  parseDeclarations(out);
  if (current_token.type != EOF_TOKEN) {
    throwError(out);
  }
  std::cout << "Total NO of errors: " << error_count << std::endl;
  out << "Total NO of errors: " << error_count << std::endl;
  return error_count == 0 ? 0 : 1;
}

unsigned int Parser::getErrorCount() const { return error_count; }

bool Parser::isDataType(TokenType token) {
  return token == INTEGER || token == SINTEGER || token == CHARACTER ||
         token == STRING || token == FLOAT || token == SFLOAT ||
         token == VOID || token == STRUCT;
}

bool Parser::isStartOfStatement(TokenType type) {
  return type == IDENTIFIER || type == CONSTANT || type == STRING_LITERAL ||
         type == CHARACTER_LITERAL ||
         (type == BRACE &&
          (current_token.text == "(" || current_token.text == "{")) ||
         type == CONDITION || type == LOOP || type == RETURN || type == BREAK;
}

bool Parser::isStartsOfLine(TokenType token) { return token == INCLUSION; }

void Parser::nextToken() {
  if (token_index + 1 < tokens.size()) {
    token_index++;
    current_token = tokens[token_index];
    line_count = current_token.line;
  } else {
    current_token = Token(line_count, "", EOF_TOKEN, false);
  }
}

void Parser::throwError(std::ofstream &out) {
  error_count++;
  std::cout << "Line : " << current_token.line
            << " Not Matched Error: Unexpected token '" << current_token.text
            << "'" << std::endl;
  out << "Line : " << current_token.line
      << " Not Matched Error: Unexpected token '" << current_token.text << "'"
      << std::endl;
  while (current_token.type != SEMICOLON && current_token.type != BRACE &&
         current_token.type != EOF_TOKEN && token_index < tokens.size()) {
    nextToken();
  }
  if (current_token.type == SEMICOLON && token_index + 1 < tokens.size()) {
    nextToken();
  }
}

void Parser::parseDeclarations(std::ofstream &out) {
  while (isDataType(current_token.type) || current_token.type == INCLUSION ||
         current_token.type == COMMENT_START ||
         current_token.type == SINGLE_LINE_COMMENT_START) {
    if (current_token.type == INCLUSION) {
      parseIncludeCommand(out);
    } else if (current_token.type == COMMENT_START ||
               current_token.type == SINGLE_LINE_COMMENT_START) {
      parseComment(out);
    } else {
      parseDeclaration(out);
    }
  }
}

void Parser::parseDeclarationList(std::ofstream &out) {
  while (isDataType(current_token.type)) {
    parseDeclaration(out);
  }
}

void Parser::parseDeclaration(std::ofstream &out) {
  int start_line = current_token.line;
  if (isDataType(current_token.type)) {
    parseTypeSpecifier(out);
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
      if (current_token.type == BRACE && current_token.text == "(") {
        in_function_scope = true;
        parseFunDec(out);
        in_function_scope = false;
        std::cout << "Line : " << start_line
                  << " Matched Rule used: fun-declaration" << std::endl;
        out << "Line : " << start_line << " Matched Rule used: fun-declaration"
            << std::endl;
      } else if (current_token.type == BRACE && current_token.text == "{") {
        parseStructDec(out);
        std::cout << "Line : " << start_line
                  << " Matched Rule used: struct-declaration" << std::endl;
        out << "Line : " << start_line
            << " Matched Rule used: struct-declaration" << std::endl;
      } else {
        parseVarDec(out);
        std::cout << "Line : " << start_line
                  << " Matched Rule used: var-declaration" << std::endl;
        out << "Line : " << start_line << " Matched Rule used: var-declaration"
            << std::endl;
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseStructDec(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "{") {
    nextToken();
    parseLocalDecs(out);
    if (current_token.type == BRACE && current_token.text == "}") {
      nextToken();
      if (current_token.type == SEMICOLON) {
        nextToken();
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseVarDec(std::ofstream &out) {
  if (current_token.type == IDENTIFIER) {
    parseIdAssign(out);
    if (current_token.type == ASSIGNMENT_OP) {
      if (!in_function_scope) {
        std::cout << "Line : " << current_token.line
                  << " Not Matched Error: Variable initialization only allowed "
                     "inside function"
                  << std::endl;
        out << "Line : " << current_token.line
            << " Not Matched Error: Variable initialization only allowed "
               "inside function"
            << std::endl;
        throwError(out);
        // error_count++;
        // nextToken();
        // parseExpression(out);
      } else {
        nextToken();
        parseExpression(out);
      }
    }
    if (current_token.type == BRACE && current_token.text == "[") {
      nextToken();
      if (current_token.type == CONSTANT) {
        nextToken();
        if (current_token.type == BRACE && current_token.text == "]") {
          nextToken();
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    }
  } else if (current_token.type == ARITHMETIC_OP && current_token.text == "*") {
    nextToken();
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
    } else {
      throwError(out);
    }
  } else if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  } else {
    throwError(out);
  }
  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseTypeSpecifier(std::ofstream &out) {
  if (isDataType(current_token.type)) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseFunDec(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "(") {
    nextToken();
    parseParams(out);
    if (current_token.type == BRACE && current_token.text == ")") {
      nextToken();
      if (current_token.type == BRACE && current_token.text == "{") {
        parseCompoundStmt(out);
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseParams(std::ofstream &out) {
  if (current_token.type == VOID) {
    nextToken();
    return;
  }
  if (isDataType(current_token.type)) {
    parseParamList(out);
  }
}

void Parser::parseParamList(std::ofstream &out) {
  parseParam(out);
  parsePList(out);
}

void Parser::parsePList(std::ofstream &out) {
  if (current_token.type == COMMA) {
    nextToken();
    parseParam(out);
    parsePList(out);
  }
}

void Parser::parseParam(std::ofstream &out) {
  if (isDataType(current_token.type)) {
    nextToken();
    if (current_token.type == IDENTIFIER) {
      parseIdAssign(out);
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseCompoundStmt(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "{") {
    nextToken();
    parseLocalDecs(out);
    parseStmtList(out);
    if (current_token.type == BRACE && current_token.text == "}") {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseLocalDecs(std::ofstream &out) {
  while (isDataType(current_token.type)) {
    nextToken();
    parseVarDec(out);
  }
}

void Parser::parseStmtList(std::ofstream &out) {
  while (isStartOfStatement(current_token.type)) {
    parseStatement(out);
  }
}

void Parser::parseStatement(std::ofstream &out) {
  int start_line = current_token.line;
  switch (current_token.type) {
  case IDENTIFIER:
  case CONSTANT:
  case STRING_LITERAL:
  case CHARACTER_LITERAL:
    parseExpressionStmt(out);
    std::cout << "Line : " << start_line
              << " Matched Rule used: expression-stmt" << std::endl;
    out << "Line : " << start_line << " Matched Rule used: expression-stmt"
        << std::endl;
    break;
  case BRACE:
    if (current_token.text == "(") {
      parseExpressionStmt(out);
      std::cout << "Line : " << start_line
                << " Matched Rule used: expression-stmt" << std::endl;
      out << "Line : " << start_line << " Matched Rule used: expression-stmt"
          << std::endl;
    } else if (current_token.text == "{") {
      parseCompoundStmt(out);
      std::cout << "Line : " << start_line
                << " Matched Rule used: compound-stmt" << std::endl;
      out << "Line : " << start_line << " Matched Rule used: compound-stmt"
          << std::endl;
    } else {
      throwError(out);
    }
    break;
  case CONDITION:
    parseSelectionStmt(out);
    std::cout << "Line : " << start_line << " Matched Rule used: selection-stmt"
              << std::endl;
    out << "Line : " << start_line << " Matched Rule used: selection-stmt"
        << std::endl;
    break;
  case LOOP:
    parseIterationStmt(out);
    std::cout << "Line : " << start_line << " Matched Rule used: iteration-stmt"
              << std::endl;
    out << "Line : " << start_line << " Matched Rule used: iteration-stmt"
        << std::endl;
    break;
  case RETURN:
  case BREAK:
    parseJumpStmt(out);
    std::cout << "Line : " << start_line << " Matched Rule used: jump-stmt"
              << std::endl;
    out << "Line : " << start_line << " Matched Rule used: jump-stmt"
        << std::endl;
    break;
  default:
    throwError(out);
  }
}

void Parser::parseExpressionStmt(std::ofstream &out) {
  if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  }
  parseExpression(out);
  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseSelectionStmt(std::ofstream &out) {
  if (current_token.type == CONDITION) {
    nextToken();
    if (current_token.type == BRACE && current_token.text == "(") {
      nextToken();
      parseExpression(out);
      if (current_token.type == BRACE && current_token.text == ")") {
        nextToken();
        parseStatement(out);
        if (current_token.type == CONDITION &&
            current_token.text == "Otherwise") {
          nextToken();
          parseStatement(out);
        }
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseIterationStmt(std::ofstream &out) {
  if (current_token.type == LOOP) {
    nextToken();
    if (current_token.text == "Reiterate") {
      if (current_token.type == BRACE && current_token.text == "(") {
        nextToken();
        parseExpression(out);
        if (current_token.type == SEMICOLON) {
          nextToken();
          parseExpression(out);
          if (current_token.type == SEMICOLON) {
            nextToken();
            parseExpression(out);
            if (current_token.type == BRACE && current_token.text == ")") {
              nextToken();
              parseStatement(out);
            } else {
              throwError(out);
            }
          } else {
            throwError(out);
          }
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    } else {
      if (current_token.type == BRACE && current_token.text == "(") {
        nextToken();
        parseExpression(out);
        if (current_token.type == BRACE && current_token.text == ")") {
          nextToken();
          parseStatement(out);
        } else {
          throwError(out);
        }
      } else {
        throwError(out);
      }
    }
  } else {
    throwError(out);
  }
}

void Parser::parseJumpStmt(std::ofstream &out) {
  if (current_token.type == RETURN) {
    nextToken();
    if (current_token.type != SEMICOLON) {
      parseExpression(out);
    }
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError(out);
    }
  } else if (current_token.type == BREAK) {
    nextToken();
    if (current_token.type == SEMICOLON) {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseExpression(std::ofstream &out) {
  if (current_token.type == IDENTIFIER) {
    parseIdAssign(out);
    if (current_token.type == ASSIGNMENT_OP) {
      nextToken();
      parseExpression(out);
    } else {
      parseSimpleExpression(out);
    }
  } else {
    parseSimpleExpression(out);
  }
}

void Parser::parseIdAssign(std::ofstream &out) {
  if (current_token.type == IDENTIFIER) {
    if (!std::isalpha(current_token.text[0]) && current_token.text[0] != '_') {
      std::cout << "Line : " << current_token.line
                << " Not Matched Error: Invalid identifier \""
                << current_token.text << "\"" << std::endl;
      out << "Line : " << current_token.line
          << " Not Matched Error: Invalid identifier \"" << current_token.text
          << "\"" << std::endl;
      throwError(out);
    } else {
      nextToken();
    }
  } else {
    throwError(out);
  }
}

void Parser::parseSimpleExpression(std::ofstream &out) {
  parseAdditiveExpression(out);
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    parseRelop(out);
    parseAdditiveExpression(out);
  }
}

void Parser::parseRelop(std::ofstream &out) {
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseAdditiveExpression(std::ofstream &out) {
  parseTerm(out);
  parseAdditiveExpressionPrime(out);
}

void Parser::parseAdditiveExpressionPrime(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    parseAddOp(out);
    parseTerm(out);
    parseAdditiveExpressionPrime(out);
  }
}

void Parser::parseAddOp(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseTerm(std::ofstream &out) {
  parseFactor(out);
  parseTermPrime(out);
}

void Parser::parseTermPrime(std::ofstream &out) {
  if (current_token.type == MULOP) {
    parseMulOp(out);
    parseFactor(out);
    parseTermPrime(out);
  }
}

void Parser::parseMulOp(std::ofstream &out) {
  if (current_token.type == MULOP) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseFactor(std::ofstream &out) {
  switch (current_token.type) {
  case BRACE:
    if (current_token.text == "(") {
      nextToken();
      parseExpression(out);
      if (current_token.type == BRACE && current_token.text == ")") {
        nextToken();
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
    break;
  case IDENTIFIER: {
    std::string id_name = current_token.text;
    nextToken();
    if (current_token.type == BRACE && current_token.text == "(") {
      parseCall(out);
    } else if (current_token.type == ACCESS_OP) {
      nextToken();
      parseIdAssign(out);
    }
  } break;
  case CONSTANT:
  case STRING_LITERAL:
  case CHARACTER_LITERAL:
    nextToken();
    break;
  case ADDOP:
    parseSignedNum(out);
    break;
  case ARITHMETIC_OP:
    if (current_token.text == "*") {
      nextToken();
      parseFactor(out);
    } else {
      throwError(out);
    }
    break;
  default:
    throwError(out);
  }
}

void Parser::parseCall(std::ofstream &out) {
  if (current_token.type == BRACE && current_token.text == "(") {
    nextToken();
    parseArgs(out);
    if (current_token.type == BRACE && current_token.text == ")") {
      nextToken();
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseArgs(std::ofstream &out) {
  if (current_token.type != BRACE || current_token.text != ")") {
    parseArgList(out);
  }
}

void Parser::parseArgList(std::ofstream &out) {
  parseExpression(out);
  parseAList(out);
}

void Parser::parseAList(std::ofstream &out) {
  if (current_token.type == COMMA) {
    nextToken();
    parseExpression(out);
    parseAList(out);
  }
}

void Parser::parseNum(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    parseSignedNum(out);
  } else if (current_token.type == CONSTANT) {
    parseUnsignedNum(out);
  } else {
    throwError(out);
  }
}

void Parser::parseSignedNum(std::ofstream &out) {
  if (current_token.type == ADDOP) {
    if (current_token.text == "+") {
      parsePosNum(out);
    } else if (current_token.text == "-") {
      parseNegNum(out);
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseUnsignedNum(std::ofstream &out) { parseValue(out); }

void Parser::parsePosNum(std::ofstream &out) {
  if (current_token.type == ADDOP && current_token.text == "+") {
    nextToken();
    parseValue(out);
  } else {
    throwError(out);
  }
}

void Parser::parseNegNum(std::ofstream &out) {
  if (current_token.type == ADDOP && current_token.text == "-") {
    nextToken();
    parseValue(out);
  } else {
    throwError(out);
  }
}

void Parser::parseValue(std::ofstream &out) {
  if (current_token.type == CONSTANT) {
    nextToken();
  } else {
    throwError(out);
  }
}

void Parser::parseComment(std::ofstream &out) {
  int start_line = current_token.line;
  if (current_token.type == COMMENT_START) {
    nextToken();
    if (current_token.type == COMMENT_CONTENT) {
      nextToken();
    }
    if (current_token.type == COMMENT_END ||
        current_token.type == INVALID_COMMENT) {
      nextToken();
    } else {
      throwError(out);
    }
  } else if (current_token.type == SINGLE_LINE_COMMENT_START) {
    nextToken();
    if (current_token.type == SINGLE_LINE_COMMENT_CONTENT) {
      nextToken();
    }
  } else {
    throwError(out);
  }
  std::cout << "Line : " << start_line << " Matched Rule used: Comment"
            << std::endl;
  out << "Line : " << start_line << " Matched Rule used: Comment" << std::endl;
}

void Parser::parseIncludeCommand(std::ofstream &out) {
  int start_line = current_token.line;
  if (current_token.type == INCLUSION) {
    nextToken();
    if (current_token.type == STRING_LITERAL ||
        current_token.type == INVALID_INCLUSION) {
      parseFName(out);
      if (current_token.type == SEMICOLON) {
        nextToken();
        std::cout << "Line : " << start_line
                  << " Matched Rule used: include_command" << std::endl;
        out << "Line : " << start_line << " Matched Rule used: include_command"
            << std::endl;
      } else {
        throwError(out);
      }
    } else {
      throwError(out);
    }
  } else {
    throwError(out);
  }
}

void Parser::parseFName(std::ofstream &out) {
  if (current_token.type == STRING_LITERAL ||
      current_token.type == INVALID_INCLUSION) {
    nextToken();
  } else {
    throwError(out);
  }
}
