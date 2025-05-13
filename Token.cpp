#include <string>
#include "Token.h"

using namespace std;

string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::CLEAR: return "CLEAR";
        case TokenType::CONDITION: return "CONDITION";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::SINTEGER: return "SINTEGER";
        case TokenType::CHARACTER: return "CHARACTER";
        case TokenType::STRING: return "STRING";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::SFLOAT: return "SFLOAT";
        case TokenType::VOID: return "VOID";
        case TokenType::LOOP: return "LOOP";
        case TokenType::RETURN: return "RETURN";
        case TokenType::ARITHMETIC_OP: return "ARITHMETIC_OP";
        case TokenType::LOGIC_OP: return "LOGIC_OP";
        case TokenType::RELATIONAL_OP: return "RELATIONAL_OP";
        case TokenType::ASSIGNMENT_OP: return "ASSIGNMENT_OP";
        case TokenType::ACCESS_OP: return "ACCESS_OP";
        case TokenType::BRACE: return "BRACE";
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::QUOTATION_MARK: return "QUOTATION_MARK";
        case TokenType::INCLUSION: return "INCLUSION";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::BREAK: return "BREAK";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMENT_START: return "COMMENT_START";
        case TokenType::COMMENT_CONTENT: return "COMMENT_CONTENT";
        case TokenType::COMMENT_END: return "COMMENT_END";
        case TokenType::SINGLE_LINE_COMMENT_START: return "SINGLE_LINE_COMMENT_START";
        case TokenType::SINGLE_LINE_COMMENT_CONTENT: return "SINGLE_LINE_COMMENT_CONTENT";
        case TokenType::INVALID: return "INVALID";
        case TokenType::INVALID_COMMENT: return "INVALID_COMMENT";
        case TokenType::INVALID_INCLUSION: return "INVALID_INCLUSION";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::CHARACTER_LITERAL: return "CHARACTER_LITERAL";
        case TokenType::UNTERMINATED_STRING: return "UNTERMINATED_STRING";
        case TokenType::UNTERMINATED_CHAR: return "UNTERMINATED_CHAR";
        case TokenType::INVALID_IDENTIFIER: return "INVALID_IDENTIFIER";
        case TokenType::ADDOP: return "ADDOP";
        case TokenType::MULOP: return "MULOP";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}
