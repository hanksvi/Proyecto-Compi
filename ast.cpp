#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case POW_OP:   return "**";
        case LS_OP:   return "<";
        case LSEQ_OP:   return "<=";
        case GR_OP:   return ">";
        case GREQ_OP:   return ">=";
        case EQ_OP:   return "==";

        default:       return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {
          if (l->isConstant && r->isConstant) {
        isConstant = true;
        
        switch (op) {
            case PLUS_OP:
                constantValue = l->constantValue + r->constantValue;
                break;
            case MINUS_OP:
                constantValue = l->constantValue - r->constantValue;
                break;
            case MUL_OP:
                constantValue = l->constantValue * r->constantValue;
                break;
            case DIV_OP:
                if (r->constantValue == 0) {
                    cerr << "Error: división por cero en plegado de constantes" << endl;
                    isConstant = false;
                } else {
                    constantValue = l->constantValue / r->constantValue;
                }
                break;
            case LS_OP:
                constantValue = (l->constantValue < r->constantValue) ? 1 : 0;
                break;
            case LSEQ_OP:
                constantValue = (l->constantValue <= r->constantValue) ? 1 : 0;
                break;
            case GR_OP:
                constantValue = (l->constantValue > r->constantValue) ? 1 : 0;
                break;
            case GREQ_OP:
                constantValue = (l->constantValue >= r->constantValue) ? 1 : 0;
                break;
            case EQ_OP:
                constantValue = (l->constantValue == r->constantValue) ? 1 : 0;
                break;
            default:
                isConstant = false;
                break;
        }
    }else {
        isConstant = false;
        constantValue = 0;
    }
}

    
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}



// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v), isFloat(false){
    isConstant = true;
    constantValue = v;
}
NumberExp::NumberExp(double v) : value(v), isFloat(true) {
    isConstant = true;
    constantValue = v;
}

NumberExp::~NumberExp() {}


// ------------------idExp ------------------
IdExp::IdExp(string v) : value(v) {
    isConstant = false;
    constantValue = 0;
}

IdExp::~IdExp() {}


Stm::~Stm(){}

PrintStm::~PrintStm(){}

AssignStm::~AssignStm(){}

IfStm::IfStm(Exp* c, Body* t, Body* e): condition(c), then(t), els(e) {}

WhileStm::WhileStm(Exp* c, Body* t): condition(c), b(t) {}


PrintStm::PrintStm(Exp* expresion){
    e=expresion;
}

AssignStm::AssignStm(string variable,Exp* expresion){
    id = variable;
    e = expresion;
}



VarDec::VarDec() {}

VarDec::~VarDec() {}

Body::Body(){
    declarations=list<VarDec*>();
    StmList=list<Stm*>();
}

Body::~Body(){}


