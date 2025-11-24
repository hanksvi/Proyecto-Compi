#include "TypeChecker.h"
#include <iostream>
#include <stdexcept>
using namespace std;


Type* NumberExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IdExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BinaryExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FcallExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BoolExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IfExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* CastExp::accept(TypeVisitor* v) { return v->visit(this); }


void AssignStm::accept(TypeVisitor* v) { v->visit(this); }
void PrintStm::accept(TypeVisitor* v) { v->visit(this); }
void ReturnStm::accept(TypeVisitor* v) { v->visit(this); }
void WhileStm::accept(TypeVisitor* v) { v->visit(this); }
void IfStm::accept(TypeVisitor* v) { v->visit(this); }

void VarDec::accept(TypeVisitor* v) { v->visit(this); }
void FunDec::accept(TypeVisitor* v) { v->visit(this); }
void Body::accept(TypeVisitor* v) { v->visit(this); }
void Program::accept(TypeVisitor* v) { v->visit(this); }

// ===========================================================
//   Constructor del TypeChecker
// ===========================================================

TypeChecker::TypeChecker() {
    intType = new Type(Type::INT);
    boolType = new Type(Type::BOOL);
    voidType = new Type(Type::VOID);
    floatType = new Type(Type::FLOAT);
    uintType = new Type(Type::UINT);

    int32Type = new Type(Type::INT32);
    float32Type = new Type(Type::FLOAT32);
    uint32Type = new Type(Type::UINT32);
    currentNameFun = "";
    currentTypeFun = nullptr;
}


// ===========================================================
//   Registrar funciones globales
// ===========================================================

void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->nombre) != functions.end()) {
        cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
        exit(0);
    }

    Type* returnType = new Type();
    if (!returnType->set_basic_type(fd->tipo)) {
        cerr << "Error: tipo de retorno no válido en función '" << fd->nombre << "'." << endl;
        exit(0);
    }

    functions[fd->nombre] = returnType;
    functionDeclarations[fd->nombre] = fd;
}



// ===========================================================
//   Método principal de verificación
// ===========================================================

void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    cout << "Revisión exitosa" << endl;
}


// ===========================================================
//   Nivel superior: Programa y Bloque
// ===========================================================

void TypeChecker::visit(Program* p){
    // Registramos funciones
    for (auto f: p->fdlist){
        add_function(f);
    }

    env.add_level();
    for (auto v: p->vdlist){
        v->accept(this);
    }
    for (auto v: p->fdlist){
        v->accept(this);
    }
    p->cuerpo->accept(this);
    env.remove_level();
}

void TypeChecker::visit(Body* b) {
    env.add_level();
    for (auto v : b->declarations)
        v->accept(this); 
    for (auto s : b->StmList)
        s->accept(this); 
    env.remove_level();
}

// ===========================================================
//   Declaraciones
// ===========================================================

void TypeChecker::visit(VarDec* v){
    Type* t = new Type();
    if (!t->set_basic_type(v->type)){
        cerr << "Error: tipo de variable no válido." << endl;
        exit(0);
    }

    for(const auto& id: v->vars){
        if(env.check(id)){
            cerr << "Error: variable '" << id << "' ya declarada." << endl;
            exit(0);
        }
        env.add_var(id, t);
    }
}

void TypeChecker::visit(FunDec* f){
    string previousName = currentNameFun;
    Type* previousType = currentTypeFun;

    currentNameFun = f->nombre;
    Type* returnType = new Type();
    if(!returnType->set_basic_type(f->tipo)){
        cerr<<"Error: tipo de retorno invalido en funcion: "<<f->nombre<<"."<<endl;
        exit(0);
    }
    currentTypeFun = returnType;
    env.add_level();
    for(int i=0; i<f->Ptipos.size(); i++){
        Type* t = new Type();
        if(!t->set_basic_type(f->Ptipos[i])){
            cerr <<"Error: tipo de parametro invalido en funcion: "<<f->nombre<<"."<<endl;
            exit(0);
        }
        env.add_var(f->Pnombres[i], t);
    }
    f->cuerpo->accept(this);
    env.remove_level();
    currentNameFun = previousName;
    currentTypeFun = previousType;
}

// ===========================================================
//   Sentencias
// ===========================================================

void TypeChecker::visit(PrintStm* stm){
    Type* t = stm->e->accept(this);
    if(!(t->match(intType) ||t->match(boolType) || t->match(floatType) || t->match(int32Type) 
        || t->match(float32Type) || t->match(uintType) || t->match(uint32Type))){
        cerr << "Error: tipo inválido en print (solo int, bool, float, float32, int32, uint o uint32)." << endl;
        exit(0);
    }

}

void TypeChecker::visit(AssignStm* stm){
    if(!env.check(stm->id)){
        cerr << "Error: variable '" << stm->id << "' no declarada." << endl;
        exit(0);
    }
    Type* idd = env.lookup(stm->id);
    Type* exp = stm->e->accept(this);
    if(!idd->match(exp)){
        cerr << "Error: tipos incompatibles en asignacion a "<<stm->id
        <<" (debe ser tipo "<<idd->toString()<<")"<< "."<< endl;
        exit(0);
    }
}

void TypeChecker::visit(ReturnStm* stm){
    if(currentNameFun.empty()){
        cerr << "Error: 'return' fuera de una funcion."<<endl;
        exit(0);
    }
    if(currentTypeFun->match(voidType)){
        if (stm->e != nullptr){
            cerr<<"Error: funcion "<<currentNameFun<<" no debe retornar nada (funcion tipo void)."<<endl;
            exit(0);
        }
        return;
    }

    // Si no es un tipo void y no retorna nada
    if(stm->e == nullptr){
        cerr <<"Error: funcion "<<currentNameFun<<"debe retornar un valor tipo "
        <<currentTypeFun->toString()<<"."<<endl;
        exit(0);
    }
    else{
        Type* t = stm->e->accept(this);
        if(!(t->match(voidType) || t->match(intType) ||t->match(boolType) || t->match(floatType) 
            || t->match(int32Type) || t->match(float32Type) || t->match(uintType) || t->match(uint32Type))){
            cerr << "Error: tipo inválido en return." << endl;
            exit(0);
        }
    }
    Type* returnType = stm->e->accept(this);
    if(!returnType->match(currentTypeFun)){
        cerr<<"Error: funcion "<<'"'<<currentNameFun<<'"'<<" debe retornar tipo "<<currentTypeFun->toString()
        <<", pero se esta retornando "<<returnType->toString()<<"."<<endl;
        exit(0);
    }
}

void TypeChecker::visit(WhileStm* stm){
    Type* t = stm->condition->accept(this);
    if(!t->match(boolType)){
        cerr << "Error: tipo inválido de condicion while (solo bool)." << endl;
        exit(0);
    }
    stm->b->accept(this);
}

void TypeChecker::visit(IfStm* stm){
    Type* t = stm->condition->accept(this);
    if(!t->match(boolType)){
        cerr << "Error: tipo inválido de condicion while (solo bool)." << endl;
        exit(0);
    }
    stm->then->accept(this);
    if(stm->els){
        stm->els->accept(this);
    }
}

// ===========================================================
//   Expresiones
// ===========================================================

Type* TypeChecker::visit(BinaryExp* e) {
    Type* left = e->left->accept(this);
    Type* right = e->right->accept(this);

    switch (e->op) {
        case PLUS_OP: 
        case MINUS_OP: 
        case MUL_OP: 
        case DIV_OP: 
            if (!(left->match(right))) {
                cerr << "Error: operación aritmética requiere operandos del mismo tipo." << endl;
                exit(0);
            }
            if (!(left->match(intType) || left->match(floatType) || left->match(int32Type) 
                    || left->match(float32Type) || left->match(uintType) || left->match(uint32Type))){
                cerr << "Error: operación aritmética requiere operandos tipo valido." << endl;
                exit(0);
                }
            return left;
        case LS_OP:    
        case LSEQ_OP:
        case GR_OP:
        case GREQ_OP:  
        case EQ_OP:
            if (!(left->match(right))) {
                cerr << "Error: operación aritmética requiere operandos del mismo tipo." << endl;
                exit(0);
            }
            return boolType;
        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
    }
}

Type* TypeChecker::visit(NumberExp* e){
    if(e->isFloat){
        return floatType;
    }
    else{
        return intType;
    }
}

Type* TypeChecker::visit(BoolExp* e){
    return boolType;
}


Type* TypeChecker::visit(FcallExp* e){
    auto it = functions.find(e->nombre);
    if (it == functions.end()){
        cerr << "Error: llamada a función no declarada '" << e->nombre << "'." << endl;
        exit(0);
    }

    FunDec* fd = functionDeclarations[e->nombre];

    if (e->argumentos.size() != fd->Ptipos.size()) {
        cerr << "Error: función '" << e->nombre << "' espera "
            << fd->Ptipos.size() << " argumentos, pero se pasaron "
            << e->argumentos.size() << "." << endl;
    exit(0);
    }

    for(int i=0; i<fd->Ptipos.size();i++){
        Type* t = new Type();
        t->set_basic_type(fd->Ptipos[i]);
        Type* actual = e->argumentos[i]->accept(this); 
        if(!t->match(actual)){
            cerr << "Error: argumento #" << i+1 << " de la función '"
                << e->nombre << "' debe ser de tipo "
                << t->toString()
                << ", pero se recibió "
                << actual->toString() << "." << endl;
             exit(0);
        }
    }
    return it->second;
}

Type* TypeChecker::visit(IdExp* e){
    if(!env.check(e->value)){
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    return env.lookup(e->value);
}

Type* TypeChecker::visit(IfExp* e){
    Type* ctype = e->condicion->accept(this);
    if(!ctype->match(boolType)){
        cerr << "Error: condición de if-expression debe ser bool."<< endl;
        exit(0);
    }
    Type* thentype = e->then->accept(this);
    Type* elstype = e->els->accept(this);
    if(!thentype->match(elstype)){
        cerr << "Error: las ramas del if-expression tienen tipos incompatibles."<< endl;
        exit(0);
    }
    return thentype;
}

Type* TypeChecker::visit(CastExp* e){
    Type* t = new Type();
    if (!t->set_basic_type(e->tipo)){
        cerr << "Error: tipo casteo invalido." << endl;
        exit(0);
    }
    return t;
}