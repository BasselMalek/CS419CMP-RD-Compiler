#include "iostream"
#include "string"
#include "parser.h"
using namespace std;
static void nextToken() { int x = 1; };
enum TokenType {
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
  CONSTANT,
  QUOTATION_MARK,
  INCLUSION,
  SINGLE_LINE_COMMENT,
  MULTI_LINE_COMMENT_START,
  MULTI_LINE_COMMENT_END,
  STRUCT,
  BREAK,
  IDENTIFIER,
  COMMA,
  COLON,
  SEMICOLON
};

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

struct Token {
  TokenType type;
  string value;
  Token(TokenType t, string val) {
    this->type = t;
    this->value = val;
  }
};

// Recursive descent parser w/ modifed right-recursive grammar in md file.
// Assumes tokens are stored in current_token, nextToken() advances the reading
// head for the token stream.

unsigned int line_count = 0, error_count = 0;
Token current_token = Token(INCLUSION, "include");

void parseIncludes() {
  // Useless function.
  // From project description an include directive is processed during scanning
  // and the content of the file will be dumbed in the token stream.
}

/*8. `params → param-list | NOReturn | ε`
9. `param-list → param p_list`
10. `p_list -> , param p_list | e`
11. `param → type-specifier ID`*/

void parseParam() {
    if (isDataType(current_token.type)) {
        nextToken();
    }
    if (current_token.type == IDENTIFIER) {
        nextToken();
        return;
    }
    else {
        throwError();
    }
}

void parsePList() {
    if (current_token.type == CLOSE_BRACKET) {
        return;
    } else if (current_token.type == COMMA) {
        nextToken();
        parseParam();
        parsePList();
        return;
    }
    else {
        throwError();
    }
}

void parseParamList() {
    parseParam();
    parsePList();
}

void parseParams() { 
    nextToken();
    if (isDataType(current_token.type))
    {
        parseParamList();
    }
    else if (current_token.type == VOID || current_token.type == CLOSE_BRACE) {
        return;
    }
    else {
        throwError();
    }
}

void parseCompoundStmt() {
};
// 7. `fun-declaration → type-specifier ID ( params ) compound-stmt | comment
// type-specifier ID`
void parseFunDec() {
  if (current_token.type == OPEN_BRACE) {
    parseParams();
    if (current_token.type == OPEN_BRACKET) {
      parseCompoundStmt();
    } 
  }
}
// 2. `declaration-list → declaration d_list`
// 3. `d_list->declaration d_list | e`
// 4. declaration → var - declaration |fun - declaration
void parseDeclarationList() {//need to figure out how to handle the recursive part of recursive descent lol
}
void parseDeclaration() {
  if (isDataType(current_token.type)) {
    nextToken();
    if (current_token.type == IDENTIFIER) {
      nextToken();
      if (current_token.type == OPEN_BRACE) {
        parseFunDec();
      } else {
        parseVarDec();
      }
    }
  }
}

int parseProgram() {
  // Entry point
  //{program → declaration-list | comment | include_command} does not make
  // sense??.
  // Will use {program → include_command program_unit}`
  // {program_unit -> declaration_list program_unit | comment program_unit}
  // Comments are ignored by the parser in general and includes are just scanned
  // into the stream so the working rule is just
  // {program_unit -> declaration_list program_unit | e}
  if (current_token.value == "CLEAR") {
    parseDeclarations();
  } else {
    cout << "Errors encountered in scanning. Could not parse." << endl;
  }
}
