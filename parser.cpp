#include "parser.h"
#include "iostream"
#include "string"
#include "vector"
using namespace std;

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
  OPEN_BRACE,
  CLOSE_BRACE,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  OPEN_PARTH,
  CLOSE_PARTH,
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

struct Token {
  TokenType type;
  string value;
  Token(TokenType t, string val) {
    this->type = t;
    this->value = val;
  }
};

unsigned int line_count = 0, error_count = 0;
std::vector<Token> tokens;
Token current_token = Token(CLEAR, "include");

bool isDataType(TokenType token) {
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

bool isStartOfStatement(TokenType type) {
  return type == IDENTIFIER || type == CONSTANT || type == OPEN_PARTH ||
         type == OPEN_BRACE || type == CONDITION || type == LOOP ||
         type == RETURN || type == BREAK;
}

bool isStartsOfLine(TokenType token) {
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

static void nextToken() {
  static int head = 0;
  current_token = tokens[++head];
  line_count += (isStartsOfLine(current_token.type)) ? 1 : 0;
}

void throwError() {
  error_count++;
  cout << "Error at line " << line_count << ": Unexpected token "
       << current_token.value << endl;
  while (current_token.type != SEMICOLON) {
    nextToken();
  }
  nextToken();
  if (isDataType(current_token.type)) {
    parseDeclaration();
  } else {
    parseStatement();
  }
}

// 1. program → declaration-list | comment | include_command
int parseProgram() {
  if (current_token.value == "CLEAR") {
    nextToken();
    parseDeclarations();
  } else {
    cout << "Invalid token stream. Rescan and try again.";
    return;
  }
}

// 2. declaration-list → declaration d_list
void parseDeclarations() {
  parseDeclaration();
  parseDeclarationList();
}

// 3. d_list -> declaration d_list | ε
void parseDeclarationList() {
  if (isDataType(current_token.type)) {
    parseDeclaration();
    parseDeclarationList();
  }
  return;
}

// 4. declaration → var-declaration | fun-declaration
void parseDeclaration() {
  if (isDataType(current_token.type)) {
    parseTypeSpecifier();
    if (current_token.type == IDENTIFIER) {
      nextToken();
      if (current_token.type == OPEN_PARTH) {
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

// 5. var-declaration → type-specifier ID ;
void parseVarDec() {
  if (isDataType(current_token.type)) {
    nextToken();
  }  
  if (current_token.type == IDENTIFIER) {
    nextToken();
  }
  if (current_token.type == SEMICOLON) {
    nextToken();
    return;
  } else if (current_token.type == COMMA) {
    nextToken();
    parseVarDec();
  } else {
    throwError();
  }
}

// 6. type-specifier → Imw | SIMw | Chj | Series | IMwf | SIMwf | NOReturn
void parseTypeSpecifier() {
  if (isDataType(current_token.type)) {
    nextToken();
  } else {
    throwError();
  }
}

// 7. fun-declaration → type-specifier ID ( params ) compound-stmt | comment
// type-specifier ID
void parseFunDec() {
  if (current_token.type == OPEN_PARTH) {
    nextToken();
    parseParams();
    if (current_token.type == CLOSE_PARTH) {
      nextToken();
      if (current_token.type == OPEN_BRACE) {
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

// 8. params → param-list | NOReturn | ε
void parseParams() {
  if (current_token.type == VOID) {
    nextToken();
  } else if (isDataType(current_token.type)) {
    parseParamList();
  }
  if (current_token.type == CLOSE_PARTH) {
    return;
  } else {
    throwError();
  }
}

// 9. param-list → param p_list
void parseParamList() {
  parseParam();
  parsePList();
}

// 10. p_list -> , param p_list | ε
void parsePList() {
  if (current_token.type == CLOSE_PARTH) {
    return;
  } else if (current_token.type == COMMA) {
    nextToken();
    parseParam();
    parsePList();
    return;
  } else {
    throwError();
  }
}

// 11. param → type-specifier ID
void parseParam() {
  if (isDataType(current_token.type)) {
    nextToken();
  }
  if (current_token.type == IDENTIFIER) {
    nextToken();
    return;
  } else {
    throwError();
  }
}

// 12. compound-stmt → { comment local-declarations statement-list } | {
// local-declarations statement-list }
void parseCompoundStmt() {
  if (current_token.type == OPEN_BRACE) {
    nextToken();
    parseLocalDecs();
    parseStmtList();
    if (current_token.type == CLOSE_BRACE) {
      nextToken();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

// 13. local-declarations → var-declaration local-declarations | ε
void parseLocalDecs() {
  if (isDataType(current_token.type)) {
    parseVarDec();
    parseLocalDecs();
  }
}

// 14. statement-list → statement  statement-list| ε
void parseStmtList() {
  if (isStartOfStatement(current_token.type)) {
    parseStatement();
    parseStmtList();
  }
}

// 15. statement → expression-stmt | compound-stmt | selection-stmt |
// iteration-stmt | jump-stmt
void parseStatement() {
  switch (current_token.type) {
  case IDENTIFIER:
  case CONSTANT:
  case OPEN_PARTH:
    parseExpressionStmt();
    break;
  case OPEN_BRACE:
    parseCompoundStmt();
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
           current_token.type != CLOSE_BRACE) {
      nextToken();
    }
  }
}

// 16. expression-stmt → expression ; | ;
void parseExpressionStmt() {
  parseExpression();
  if (current_token.type == SEMICOLON) {
    nextToken();
  } else {
    throwError();
  }
}

// 17. selection-stmt → IfTrue ( expression ) statement | IfTrue ( expression )
// statement Otherwise statement
void parseSelectionStmt() {
  if (current_token.type == CONDITION) {
    nextToken();
    if (current_token.type == OPEN_PARTH) {
      nextToken();
      parseExpression();
      if (current_token.type == CLOSE_PARTH) {
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
  }
}

// 18. iteration-stmt → RepeatWhen ( expression ) statement | Reiterate (
// expression ; expression ; expression ) statement
void parseIterationStmt() {
  if (current_token.type == LOOP) {
    nextToken();
    if (current_token.value == "RepeatWhen") {
      if (current_token.type == OPEN_PARTH) {
        nextToken();
        parseExpression();
        if (current_token.type == CLOSE_PARTH) {
          nextToken();
          parseStatement();
        } else {
          throwError();
        }
      } else {
        throwError();
      }
    } else if (current_token.value == "Reiterate") {
      if (current_token.type == OPEN_PARTH) {
        nextToken();
        parseExpression();
        if (current_token.type == SEMICOLON) {
          nextToken();
          parseExpression();
          if (current_token.type == SEMICOLON) {
            nextToken();
            parseExpression();
            if (current_token.type == CLOSE_PARTH) {
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
  }
}

// 19. jump-stmt → Turnback expression ; | Stop ;
void parseJumpStmt() {
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
      return;
    } else {
      throwError();
    }
  }
}

// 20. expression → id-assign = expression | simple-expression | id-assign
void parseExpression() {
  if (current_token.type == IDENTIFIER) {
    parseIdAssign();
    if (current_token.type == ASSIGNMENT_OP) {
      nextToken();
      parseExpression();
    }
  } else {
    parseSimpleExpression();
  }
}

// 21. id-assign → ID
void parseIdAssign() {
  if (current_token.type == IDENTIFIER) {
    nextToken();
  } else {
    throwError();
  }
}

// 22. simple-expression → additive-expression relop additive-expression |
// additive-expression
void parseSimpleExpression() {
  parseAdditiveExpression();
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    parseRelop();
    parseAdditiveExpression();
  }
}

// 23. relop → <= | < | > | >= | == | != | && | ||
void parseRelop() {
  if (current_token.type == RELATIONAL_OP || current_token.type == LOGIC_OP) {
    nextToken();
  } else {
    throwError();
  }
}

// 24. additive-expression →  term additive-expression'
// additive-expression' → addop term additive-expression' | ε
void parseAdditiveExpression() {
  parseTerm();
  parseAdditiveExpressionPrime();
}

void parseAdditiveExpressionPrime() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    parseAddOp();
    parseTerm();
    parseAdditiveExpressionPrime();
  }
}

// 25. addop → + | -
void parseAddOp() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    nextToken();
  } else {
    throwError();
  }
}

// 26. term → factor term'
// term' → mulop factor term' | ε
void parseTerm() {
  parseFactor();
  parseTermPrime();
}

// 27. mulop → * | /
void parseMulOp() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "*" || current_token.value == "/")) {
    nextToken();
  } else {
    throwError();
  }
}

void parseTermPrime() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "*" || current_token.value == "/")) {
    parseMulOp();
    parseFactor();
    parseTermPrime();
  }
}

// 28. factor → ( expression ) | id-assign | call | num
void parseFactor() {
  switch (current_token.type) {
  case OPEN_PARTH:
    nextToken();
    parseExpression();
    if (current_token.type == CLOSE_PARTH) {
      nextToken();
    } else {
      throwError();
    }
    break;
  case IDENTIFIER:
    nextToken();
    if (current_token.type == OPEN_PARTH) {
      parseCall();
    }
    break;
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

// 29. call → ID ( args )
void parseCall() {
  if (current_token.type == OPEN_PARTH) {
    nextToken();
    parseArgs();
    if (current_token.type == CLOSE_PARTH) {
      nextToken();
    } else {
      throwError();
    }
  } else {
    throwError();
  }
}

// 30. args → arg-list | ε
void parseArgs() {
  if (current_token.type != CLOSE_PARTH) {
    parseArgList();
  }
}

// 31. arg-list → expression a_list
void parseArgList() {
  parseExpression();
  parseAList();
}

// 32. a_list -> , expression a_list | ε
void parseAList() {
  if (current_token.type == COMMA) {
    nextToken();
    parseExpression();
    parseAList();
  }
}

// 33. num → Signed num | Unsigned num
void parseNum() {
  if (current_token.type == ARITHMETIC_OP &&
      (current_token.value == "+" || current_token.value == "-")) {
    parseSignedNum();
  } else if (current_token.type == CONSTANT) {
    parseUnsignedNum();
  } else {
    throwError();
  }
}

// 34. Unsigned num → value
void parseUnsignedNum() { parseValue(); }

// 35. Signed num → pos-num | neg-num
void parseSignedNum() {
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

// 36. pos-num → + value
void parsePosNum() {
  if (current_token.type == ARITHMETIC_OP && current_token.value == "+") {
    nextToken();
    parseValue();
  } else {
    throwError();
  }
}

// 37. neg-num → - value
void parseNegNum() {
  if (current_token.type == ARITHMETIC_OP && current_token.value == "-") {
    nextToken();
    parseValue();
  } else {
    throwError();
  }
}

// 38. value → INT_NUM | FLOAT_NUM
void parseValue() {
  if (current_token.type == CONSTANT) {
    nextToken();
  } else {
    throwError();
  }
}

// 39. comment → /@ STR @/ | /^ STR
// As per the docs comments are to be ignored.
void parseComment() { nextToken(); }

// 40. include_command → include ( F_name.txt );
// As per the docs include commands should be handled by the scanner where the
// files are dumped into the stream.
void parseIncludeCommand() {}

// 41. F_name → STR
void parseFName() {
  if (current_token.type == STRING) {
    nextToken();
  } else {
    throwError();
  }
}