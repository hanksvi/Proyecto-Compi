#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include "environment.h"
#include "TypeChecker.h"
using namespace std;

class BinaryExp;
class NumberExp;
class Program;
class PrintStm;
class WhileStm;
class IfStm;
class AssignStm;
class Body;
class Vardec;
class FcallExp;
class ReturnStm;
class FunDec;
class IfExp;
class CastExp;
class BoolExp;
class FcallStm;
class Visitor {
public:
    // 
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(IfExp* exp) = 0;
    virtual int visit(CastExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;

    virtual int visit(Program* p) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(FcallStm* stm) = 0;
    virtual int visit(Body* body) = 0;
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(FcallExp* fcall) = 0;
    virtual int visit(ReturnStm* r) = 0;
    virtual int visit(FunDec* fd) = 0;
};


class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
public:
    GenCodeVisitor(std::ostream& out) : out(out) {}
    int generar(Program* program);
    Environment<int> env;
    TypeChecker tipe;

    // Obtiene el tipo como string
    string obtenerTipoString(Exp* exp);

    unordered_map<string,int> fun_reserva;
    unordered_map<string, bool> memoriaGlobal;
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    bool enNivelGlobal = true;
    string nombreFuncion;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(IfExp* exp) override;
    int visit(CastExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(FcallExp* exp) override;

    int visit(Program* p) override ;
    int visit(Body* body) override;
    int visit(VarDec* vd) override;
    int visit(FunDec* fd) override;

    int visit(PrintStm* stm) override;
    int visit(AssignStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(FcallStm* stm) override;
    
};

class PrintVisitor: public Visitor{
private:
    int indentLevel = 0;
    void indent(){
        for (int i=0; i<indentLevel; i++){
            cout<<"  ";
        }
    }
public:
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(IfExp* exp) override;
    int visit(BoolExp* exp) override;

    int visit(CastExp* exp) override;
    int visit(Program* p) override ;
    int visit(PrintStm* stm) override;
    int visit(AssignStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(Body* body) override;
    int visit(VarDec* vd) override;
    int visit(FcallExp* fcall) override;
    int visit(ReturnStm* r) override;
    int visit(FunDec* fd) override;
    int visit(FcallStm* stm) override;

    void imprimir(Program* program);
};

class EVALVisitor: public Visitor{
public:
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(IfExp* exp) override;
    int visit(BoolExp* exp) override;

    int visit(CastExp* exp) override;
    int visit(Program* p) override ;
    int visit(PrintStm* stm) override;
    int visit(AssignStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(Body* body) override;
    int visit(VarDec* vd) override;
    int visit(FcallExp* fcall) override;
    int visit(ReturnStm* r) override;
    int visit(FunDec* fd) override;
    int visit(FcallStm* stm) override;
    void interprete(Program* program);
};




#endif // VISITOR_H