#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"
#include <vector>
using namespace std;

class Scanner {
private:
    string input;
    int first;
    int current;
    vector<int> indentStack;
    bool LineStart;
    int pendingDedent;
public:
    // Constructor
    Scanner(const char* in_s);

    // Retorna el siguiente token
    Token* nextToken();

    // Destructor
    ~Scanner();

};

// Ejecutar scanner
int ejecutar_scanner(Scanner* scanner,const string& InputFile);

#endif // SCANNER_H