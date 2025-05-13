#include "Lexer.h"
#include "Parser.h"
#include "helpers.h"
#include "Token.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <iomanip>

using namespace std;

int main() {
    string fileName;
    cout << "Enter the file name: ";
    getline(cin, fileName);

    string sourceCode = readFile(fileName);
    Lexer lexer(sourceCode);
    vector<Token> tokens = lexer.tokenize();

    ofstream out("tokens.txt");
    cout << left << setw(8) << "Line" << "| " << setw(15) << "Lexeme" << "| " << "Token Type\n";
    cout << string(50, '-') << "\n";
    out << left << setw(8) << "Line" << "| " << setw(15) << "Lexeme" << "| " << "Token Type\n";
    out << string(50, '-') << "\n";

    int errorCount = 0;
    for (const auto& token : tokens) {
        string errorNote = token.error ? " (Error)" : "";
        cout << left << setw(8) << token.line << "| " << setw(15) << token.text << "| " << tokenTypeToString(token.type) << errorNote << "\n";
        out  << left << setw(8) << token.line << "| " << setw(15) << token.text << "| " << tokenTypeToString(token.type) << errorNote << "\n";
        if (token.error) errorCount++;
    }

    cout << "\nTotal Number of lexical errors: " << errorCount << "\n";
    out << "\nTotal Number of lexical errors: " << errorCount << "\n";

    cout << "\nParser Results:\n";
    cout << string(50, '-') << "\n";
    out << "\nParser Results:\n";
    out << string(50, '-') << "\n";

    Parser parser;
    parser.setTokens(tokens);
    parser.parse(out);

    out.close();
    return 0;
}
