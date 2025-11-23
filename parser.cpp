#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

// Quitar todos las lineas vacias
void Parser::skipEmptyLines() {
    while(check(Token::NEWLINE)) {
        advance();
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();
    skipEmptyLines();
    if(check(Token::VAR)) {
        p->vdlist.push_back(parseVarDec());
        while(match(Token::NEWLINE)) {
            skipEmptyLines();
            if(check(Token::VAR)) {
                p->vdlist.push_back(parseVarDec());
            }
        }
    }
    skipEmptyLines();
    if(check(Token::PROC)) {
        p->fdlist.push_back(parseFunDec());
        while(check(Token::PROC)){
                p->fdlist.push_back(parseFunDec());
            }
        }
    skipEmptyLines();
    p->cuerpo = parseBody();

    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico: program");
    }
    
    cout << "Parseo exitoso" << endl;
    return p;
}

VarDec* Parser::parseVarDec(){
    VarDec* vd = new VarDec();
    match(Token::VAR);
    match(Token::ID);
    vd->vars.push_back(previous->text);
    while(match(Token::COMA)) {
        match(Token::ID);
        vd->vars.push_back(previous->text);
    }
    match(Token::COLON);
    match(Token::ID);
    vd->type = previous->text;
    return vd;
}

FunDec *Parser::parseFunDec() {
    FunDec* fd = new FunDec();
    match(Token::PROC);
    match(Token::ID);
    fd->nombre = previous->text;
    match(Token::LPAREN);
    if(check(Token::ID)) {
        match(Token::ID);
        fd->Pnombres.push_back(previous->text);
        match(Token::COLON);
        match(Token::ID);
        fd->Ptipos.push_back(previous->text);
        while(match(Token::COMA)) {
            match(Token::ID);
            fd->Pnombres.push_back(previous->text);
            match(Token::COLON);
            match(Token::ID);
            fd->Ptipos.push_back(previous->text);
        }
    }
    match(Token::RPAREN);
    match(Token::COLON);
    match(Token::ID);
    fd->tipo = previous->text;
    match(Token::ASSIGN);
    match(Token::NEWLINE);
    match(Token::INDENT);
    fd->cuerpo = parseBody();
    match(Token::DEDENT);
    return fd;
}



Body* Parser::parseBody(){
    
    Body* b = new Body();
    if(check(Token::VAR)) {
        b->declarations.push_back(parseVarDec());
        while(match(Token::NEWLINE)) {
            skipEmptyLines();
            if(check(Token::VAR)) {
                b->declarations.push_back(parseVarDec());
            }
        }
    }
    b->StmList.push_back(parseStm());
    
    while (match(Token::NEWLINE) || previous->type == Token::DEDENT) {
        skipEmptyLines();
        if(check(Token::ID) || check(Token::ECHO) || check(Token::IF) || check(Token::WHILE)
         || check(Token::RETURN)) {
        
        b->StmList.push_back(parseStm());
        }
        else {
            break;
        }
    }
    
    return b;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;
    if(match(Token::ID)){
        variable = previous->text;
        match(Token::ASSIGN);
        

        e = parseCE();
        
        return new AssignStm(variable,e);
    }
    else if(match(Token::ECHO)){
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        return new PrintStm(e);
    }
    else if(match(Token::RETURN)) {
        ReturnStm* r  = new ReturnStm();
        match(Token::LPAREN);
        r->e = parseCE();
        match(Token::RPAREN);
        return r;
    }
    else if (match(Token::IF)) {
        e = parseCE();
        match(Token:: COLON);
        match(Token::NEWLINE);
        if (!match(Token::INDENT)) {
            cout << "Error: se esperaba 'identacion de if' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::DEDENT)) {
            cout << "Error: se esperaba 'cierre de indentacion de if' después de la expresión." << endl;
            exit(1);
        }
        if (match(Token::ELSE)) 
        {
            match(Token::COLON);
            match(Token::NEWLINE);
            if (!match(Token::INDENT)) {
            cout << "Error: se esperaba 'indentacion de else' después de la expresión." << endl;
            exit(1);
            }
            fb = parseBody();
            if (!match(Token::DEDENT)) {
            cout << "Error: se esperaba 'cierre de indentacion de else' después de la expresión." << endl;
            exit(1);
            }
        }
        a = new IfStm(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        e = parseCE();
        match(Token::COLON);
        match(Token::NEWLINE);
        if (!match(Token::INDENT)) {
            cout << "Error: se esperaba 'indentacion' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::DEDENT)) {
            cout << "Error: se esperaba 'cierre de indentacion' después de la expresión." << endl;
            exit(1);
        }
        a = new WhileStm(e, tb);
    }
    else{
        throw runtime_error("Error sintáctico stm");
    }
    return a;
}

Exp* Parser::parseCE() {
    Exp* l = parseBE();
    if (match(Token::LS)) {
        BinaryOp op = LS_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    else if(match(Token::LSEQ)) {
        BinaryOp op = LSEQ_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    else if(match(Token::GR)) {
        BinaryOp op = GR_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    else if(match(Token::GREQ)) {
        BinaryOp op = GREQ_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    else if(match(Token::EQ)) {
        BinaryOp op = EQ_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }

    return l;
}



Exp* Parser::parseBE() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    Exp* e;
    string nom;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0);
    }
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        nom = previous->text;
        
        if(check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nom;
            
            if (!check(Token::RPAREN)) {
                fcall->argumentos.push_back(parseCE());
                while(match(Token::COMA)) {
                    fcall->argumentos.push_back(parseCE());
                }
            }
            
            match(Token::RPAREN);
            return fcall;
        }
        else {
            return new IdExp(nom);
            }
    }
    else {
        throw runtime_error("Error sintáctico: parseF");
    }
}
