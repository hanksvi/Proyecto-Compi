#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;

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
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    double value;
    bool isFloat;
    int accept(Visitor* visitor);
    NumberExp(int v);
    NumberExp(double v);
    ~NumberExp();
};

// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();
};



class IfExp: public Exp {
public:
    Exp* condicion;
    Exp* then;
    Exp* els;
    IfExp(Exp* condicion, Exp* then, Exp* els) : condicion(condicion), then(then), els(els) {};
    int accept(Visitor* visitor);
    ~IfExp(){};
};

class CastExp: public Exp {
public:
    string tipo;
    Exp* e;
    CastExp(string tipo, Exp* exp): tipo(tipo), e(exp){};
    int accept(Visitor* visitor);
    ~CastExp(){};
};

class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class VarDec{
public:
    string type;
    vector<string> vars;
    VarDec();
    int accept(Visitor* visitor);
    ~VarDec();
};


class Body{
public:
    list<Stm*> StmList;
    list<VarDec*> declarations;
    int accept(Visitor* visitor);
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
    ~IfStm(){};
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStm(){};
};



class AssignStm: public Stm {
public:
    string id;
    Exp* e;
    AssignStm(string, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
};






class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(){};
    ~ReturnStm(){};
    int accept(Visitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp(){};
    ~FcallExp(){};
};




class FunDec{
public:
    string nombre;
    string tipo;
    Body* cuerpo;
    vector<string> Ptipos;
    vector<string> Pnombres;
    int accept(Visitor* visitor);
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
};



#endif // AST_H
