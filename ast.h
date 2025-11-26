#ifndef AST_H
#define AST_H

#include <string>
#include <unordered_map>
#include <list>
#include <ostream>
#include <vector>
#include "semantic_types.h"
using namespace std;

class Visitor;
class VarDec;
class TypeVisitor;
// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    POW_OP,
    LS_OP,
    LSEQ_OP,
    GR_OP,
    GREQ_OP,
    EQ_OP
};

// Clase abstracta Exp
class Exp {
public:
    bool isConstant;
    double constantValue;
    virtual int  accept(Visitor* visitor) = 0;
    virtual Type* accept(TypeVisitor* visitor) = 0; 
    virtual ~Exp() = 0;  
    static string binopToChar(BinaryOp op);  
    Exp() : isConstant(false), constantValue(0) {}
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    double value;
    bool isFloat;
    string specificType;

    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);


    NumberExp(int v);
    NumberExp(double v);


    NumberExp(int v, string type)
        : value(v), isFloat(false), specificType(type) {
        isConstant = true;
        constantValue = v;
    }

    NumberExp(double v, string type)
        : value(v), isFloat(true), specificType(type) {
        isConstant = true;
        constantValue = v;
    }

    ~NumberExp();
};





// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    IdExp(string v);
    ~IdExp();
};



class IfExp: public Exp {
public:
    Exp* condicion;
    Exp* then;
    Exp* els;
    IfExp(Exp* condicion, Exp* then, Exp* els) : condicion(condicion), then(then), els(els) {

        if (condicion->isConstant) {
            if (condicion->constantValue != 0) {

                if (then->isConstant) {
                    isConstant = true;
                    constantValue = then->constantValue;
                } else {
                    isConstant = false;
                }
            } else {
                // Condición falsa
                if (els->isConstant) {
                    isConstant = true;
                    constantValue = els->constantValue;
                } else {
                    isConstant = false;
                }
            }
        } 

        else if (then->isConstant && els->isConstant && 
                then->constantValue == els->constantValue) {
            isConstant = true;
            constantValue = then->constantValue;
        } else {
            isConstant = false;
            constantValue = 0;
        }
    };
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    ~IfExp(){};
};

class CastExp: public Exp {
public:
    string tipo;
    Exp* e;
    CastExp(string tipo, Exp* exp): tipo(tipo), e(exp){
        if (exp->isConstant) {
            isConstant = true;
            constantValue = exp->constantValue;
            

            if (tipo == "int" || tipo == "int32") {
                constantValue = (int)constantValue;
            } else if (tipo == "bool") {
                constantValue = (constantValue != 0) ? 1 : 0;
            }

        } else {
            isConstant = false;
            constantValue = 0;
        }
    };
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    ~CastExp(){};
};

class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual void accept(TypeVisitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class VarDec{
public:
    string type;
    vector<string> vars;
    VarDec();
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    ~VarDec();
};


class Body{
public:
    list<Stm*> StmList;
    list<VarDec*> declarations;
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    Body();
    ~Body();
};




class IfStm: public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStm(Exp* condition, Body* then, Body* els);
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    ~IfStm(){};
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    ~WhileStm(){};
};

class FcallStm: public Stm{
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    FcallStm(){};
    ~FcallStm(){};
};

class AssignStm: public Stm {
public:
    string id;
    Exp* e;
    AssignStm(string, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
};






class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(){};
    ~ReturnStm(){};
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    FcallExp(){
        isConstant = false;
        constantValue = 0;
    };
    ~FcallExp(){};
};

class BoolExp : public Exp {
public:
    int valor;

    BoolExp(){
        isConstant = true;
         constantValue = 0;
    };
    ~BoolExp(){};

    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
};


class FunDec{
public:
    string nombre;
    string tipo;
    Body* cuerpo;
    vector<string> Ptipos;
    vector<string> Pnombres;
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    FunDec(){};
    ~FunDec(){};
};

class Program{
public:
    list<VarDec*> vdlist;
    list<FunDec*> fdlist;
    Body* cuerpo;
    Program(){};
    ~Program(){};
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
};



#endif // AST_H
