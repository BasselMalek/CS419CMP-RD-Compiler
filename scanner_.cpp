#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

// Structure to hold token information.
struct Token {
    int line;
    string text;
    string type;
    bool error;
};

// The Lexer class implements the lexical analysis.
class Lexer {
public:
    Lexer(const string& sourceCode)
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
            {"int", "Type"}
        };
    }

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < source.size()) {
            skipWhitespace();
            if (pos >= source.size()) break;

            char current = peek();

            // ❌ Explicitly reject C-style comments
            if (current == '/' && (peek(1) == '*' || peek(1) == '/')) {
                Token token;
                token.line = line;
                token.text = string(1, current) + peek(1);
                token.type = "Invalid Comment";
                token.error = true;
                get(); get();  // consume invalid pattern
                tokens.push_back(token);
                continue;
            }

            // ✅ Custom multi-line or single-line comment
            if (current == '/' && (peek(1) == '@' || peek(1) == '^')) {
                vector<Token> commentTokens = lexComment();
                tokens.insert(tokens.end(), commentTokens.begin(), commentTokens.end());
                continue;
            }
            else if (current == '"') {
                tokens.push_back(lexString());
                continue;
            }
            else if (current == '\'') {
                tokens.push_back(lexChar());
                continue;
            }
            else if (isalpha(current) || current == '_') {
                tokens.push_back(lexIdentifierOrKeyword());
                continue;
            }
            else if (isdigit(current)) {
                tokens.push_back(lexNumber());
                continue;
            }
            else {
                tokens.push_back(lexOperatorOrPunctuation());
                continue;
            }
        }
        return tokens;
    }

private:
    string source;
    size_t pos;
    int line;
    unordered_map<string, string> keywords;

    char peek(int n = 0) {
        if (pos + n < source.size())
            return source[pos + n];
        return '\0';
    }

    char get() {
        char c = source[pos++];
        if (c == '\n')
            line++;
        return c;
    }

    void skipWhitespace() {
        while (pos < source.size() && isspace(source[pos])) {
            if (source[pos] == '\n')
                line++;
            pos++;
        }
    }

    Token lexIdentifierOrKeyword() {
        Token token;
        token.line = line;
        token.error = false;
        string result;

        while (pos < source.size() && (isalnum(peek()) || peek() == '_')) {
            result.push_back(get());
        }

        if (result == "Repeat") {
            size_t tempPos = pos;
            while (tempPos < source.size() && isspace(source[tempPos]))
                tempPos++;
            string nextWord;
            while (tempPos < source.size() && (isalnum(source[tempPos]) || source[tempPos] == '_')) {
                nextWord.push_back(source[tempPos++]);
            }
            if (nextWord == "When") {
                pos = tempPos;
                result += " " + nextWord;
                token.type = "Loop";
                token.text = result;
                return token;
            }
        }

        if (keywords.find(result) != keywords.end()) {
            token.type = keywords[result];
        } else {
            token.type = "Identifier";
        }
        token.text = result;
        return token;
    }

    Token lexNumber() {
        Token token;
        token.line = line;
        token.error = false;
        string numberStr;
        bool seenDot = false;

        while (pos < source.size() && (isalnum(peek()) || peek() == '.')) {
            char c = peek();
            if (c == '.') {
                if (seenDot) token.error = true;  // multiple dots
                seenDot = true;
            }
            if (isalpha(c)) {
                token.error = true;
            }
            numberStr.push_back(get());
        }

        token.text = numberStr;
        token.type = token.error ? "Invalid Identifier" : "Constant";
        return token;
    }

    Token lexString() {
        Token token;
        token.line = line;
        token.error = false;
        string str;
        get(); // consume opening "

        while (pos < source.size() && peek() != '"') {
            str.push_back(get());
        }

        if (peek() == '"') {
            get(); // consume closing "
            token.type = "String Literal";
        } else {
            token.type = "Unterminated String";
            token.error = true;
        }

        token.text = str;
        return token;
    }

    Token lexChar() {
        Token token;
        token.line = line;
        token.error = false;
        string str;
        get(); // consume opening '

        if (pos < source.size() && peek() != '\'') {
            str.push_back(get());
        }

        if (peek() == '\'') {
            get(); // consume closing '
            token.type = "Character Literal";
        } else {
            token.type = "Unterminated Char";
            token.error = true;
        }

        token.text = str;
        return token;
    }

    Token lexOperatorOrPunctuation() {
        Token token;
        token.line = line;
        token.error = false;
        char current = get();
        string op(1, current);

        switch (current) {
            case '=':
                if (peek() == '=') { op += get(); token.type = "Relational Operator"; }
                else token.type = "Assignment Operator";
                break;
            case '<':
            case '>':
                if (peek() == '=') { op += get(); }
                token.type = "Relational Operator";
                break;
            case '!':
                if (peek() == '=') { op += get(); token.type = "Relational Operator"; }
                else { token.type = "Unknown Operator"; token.error = true; }
                break;
            case '&':
                if (peek() == '&') { op += get(); token.type = "Logic Operator"; }
                else token.type = "Ampersand";
                break;
            case '|':
                if (peek() == '|') { op += get(); token.type = "Logic Operator"; }
                else { token.type = "Unknown Operator"; token.error = true; }
                break;
            case '-':
                if (peek() == '>') { op += get(); token.type = "Access Operator"; }
                else token.type = "Addop";
                break;
            case '+': token.type = "Addop"; break;
            case '*': token.type = "Mulop"; break;
            case '/': token.type = "Mulop"; break;
            case '~': token.type = "Logic Operator"; break;
            case '(': case ')': case '{': case '}': case '[': case ']':
                token.type = "Braces"; break;
            case ';': token.type = "Semicolon"; break;
            case ',': token.type = "Comma"; break;
            default:
                token.type = "Unknown";
                token.error = true;
                break;
        }

        token.text = op;
        return token;
    }

    vector<Token> lexComment() {
        vector<Token> commentTokens;
        Token tokenStart;
        tokenStart.line = line;
        tokenStart.error = false;

        char second = peek(1);
        if (second == '@') {
            tokenStart.text = "/@";
            tokenStart.type = "Comment Start";
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
            tokenContent.type = "Comment Content";
            commentTokens.push_back(tokenContent);

            Token tokenEnd;
            tokenEnd.line = line;
            tokenEnd.error = false;
            if (peek() == '@' && peek(1) == '/') {
                tokenEnd.text = "@/"; tokenEnd.type = "Comment End";
                get(); get();
            } else {
                tokenEnd.text = ""; tokenEnd.type = "Unterminated Comment";
                tokenEnd.error = true;
            }
            commentTokens.push_back(tokenEnd);
        } else if (second == '^') {
            tokenStart.text = "/^";
            tokenStart.type = "Single Line Comment Start";
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
            tokenContent.type = "Single Line Comment Content";
            commentTokens.push_back(tokenContent);
        }
        return commentTokens;
    }
};

// Utility to read file content into a string
string readFile(const string &fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file \"" << fileName << "\"" << endl;
        exit(EXIT_FAILURE);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    string fileName;
    cout << "Enter the file name: ";
    getline(cin, fileName);

    string sourceCode = readFile(fileName);
    Lexer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    int errorCount = 0;
    for (const auto& token : tokens) {
        cout << "Line: " << token.line
             << (token.error ? " Error in" : "") << " Token Text: " << token.text
             << " Token Type: " << token.type << "\n";
        if (token.error) errorCount++;
    }
    cout << "Total Number of errors: " << errorCount << "\n";
    return 0;
}
