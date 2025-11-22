#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        POW,     // **
        LPAREN,  // (
        RPAREN,  // )
        SQRT,    // sqrt
        NUM,     // Número
        ID,      // ID
        LS,   // <
        LSEQ, // <=
        GR, // >
        GREQ, // >=
        EQ, // ==
        PROC,   // proc = fun
        RETURN, // return
        ASSIGN, // =
        ECHO,  // echo = print
        IF, // if
        WHILE,  //while
        NEWLINE,
        INDENT,
        DEDENT,
        ELSE,
        VAR,
        COMA,
        TRUE,
        FALSE,
        COLON,
        ////////
        //ENDWHILE,
        //ENDIF,
        //THEN,
        //FUN,
        //ENDFUN,
        //SEMICOL,
        //PRINT,
        //LE,
        //DO,
        END
        ERR,     // Error

    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H