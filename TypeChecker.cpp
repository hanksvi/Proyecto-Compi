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
void FcallStm::accept(TypeVisitor* v) { v->visit(this); }

void VarDec::accept(TypeVisitor* v) { v->visit(this); }
void FunDec::accept(TypeVisitor* v) { v->visit(this); }
void Body::accept(TypeVisitor* v) { v->visit(this); }
void Program::accept(TypeVisitor* v) { v->visit(this); }


//   Constructor del TypeChecker

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



//   Registrar funciones globales

int TypeChecker::getSizeOfType(const string& typeStr) {
    // Retorna el tamaño en bytes de un tipo
    if (typeStr == "int" || typeStr == "float" || typeStr == "uint" || typeStr == "bool") {
        return 8; 
    } else if (typeStr == "int32" || typeStr == "float32" || typeStr == "uint32") {
        return 4; 
    } else {
        return 8; 
    }
}

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

// Obtener tipo de expresiones
Type* TypeChecker::getExpressionType(Exp* exp) {
    auto it = expressionTypes.find(exp);
    if (it != expressionTypes.end()) {
        return it->second;
    }
    
    // Si no está en cache, retornar tipo por defecto
    cerr << "Advertencia: tipo de expresión no encontrado" << endl;
    return intType;
}


//   Método principal de verificación


void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    cout << "Revisión exitosa" << endl;
}



//   Nivel superior: Programa y Bloque

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


//   Declaraciones


void TypeChecker::visit(VarDec* v){
    Type* t = new Type();
    int sizeOfType = getSizeOfType(v->type);
    locales += v->vars.size() * sizeOfType;
    if (!t->set_basic_type(v->type)){
        cerr << "Error: tipo de variable no válido." << endl;
        exit(0);
    }

    for(const auto& id: v->vars){
        if(env.check_local(id)){
            cerr << "Error: variable '" << id << "' ya declarada." << endl;
            exit(0);
        }
        env.add_var(id, t);
    }
}

void TypeChecker::visit(FunDec* f){
    string previousName = currentNameFun;
    Type* previousType = currentTypeFun;

    // cuenta de variables en funcion (en bytes)
    int parametros = 0;
    for (const auto& ptype : f->Ptipos) {
        parametros += getSizeOfType(ptype);
    }
    locales = 0;

    currentNameFun = f->nombre;
    Type* returnType = new Type();
    if(!returnType->set_basic_type(f->tipo)){
        cerr<<"Error: tipo de retorno invalido en funcion: "<<f->nombre<<"."<<endl;
        exit(0);
    }
    currentTypeFun = returnType;
    for(int i=0; i<f->Ptipos.size(); i++){
        Type* t = new Type();
        if(!t->set_basic_type(f->Ptipos[i])){
            cerr <<"Error: tipo de parametro invalido en funcion: "<<f->nombre<<"."<<endl;
            exit(0);
        }
        env.add_var(f->Pnombres[i], t);
    }
    f->cuerpo->accept(this);
    
    // CORRECCIÓN: Calcular slots correctamente con redondeo hacia arriba
    int totalBytes = parametros + locales;
    fun_locales[f->nombre] = (totalBytes + 7) / 8;  // Redondeo hacia arriba a slots de 8 bytes

    currentNameFun = previousName;
    currentTypeFun = previousType;
}

//   Sentencias


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
    
    // Permitir coerción de tipos numéricos compatibles
    bool compatible = false;
    
    if(idd->match(exp)) {
        compatible = true;
    }
    // Permitir coerción de int a int32, uint, uint32
    else if((idd->toString() == "int32" || idd->toString() == "uint32" || idd->toString() == "uint") && 
            (exp->toString() == "int" || exp->toString() == "uint" || exp->toString() == "int32" || exp->toString() == "uint32")) {
        compatible = true;
    }
    // Permitir coerción de float a float32
    else if((idd->toString() == "float32") && 
            (exp->toString() == "float" || exp->toString() == "float32")) {
        compatible = true;
    }
    // Permitir coerción entre tipos flotantes e int a float
    else if((idd->toString() == "float" || idd->toString() == "float32") && 
            (exp->toString() == "float" || exp->toString() == "float32" || exp->toString() == "int" || 
             exp->toString() == "int32" || exp->toString() == "uint" || exp->toString() == "uint32")) {
        compatible = true;
    }
    
    if(!compatible){
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

void TypeChecker::visit(FcallStm* stm){
    auto it = functions.find(stm->nombre);
    if (it == functions.end()){
        cerr << "Error: llamada a función no declarada '" << stm->nombre << "'." << endl;
        exit(0);
    }

    FunDec* fd = functionDeclarations[stm->nombre];

    if (stm->argumentos.size() != fd->Ptipos.size()) {
        cerr << "Error: función '" << stm->nombre << "' espera "
            << fd->Ptipos.size() << " argumentos, pero se pasaron "
            << stm->argumentos.size() << "." << endl;
    exit(0);
    }

    for(int i=0; i<fd->Ptipos.size();i++){
        Type* t = new Type();
        t->set_basic_type(fd->Ptipos[i]);
        Type* actual = stm->argumentos[i]->accept(this); 
        if(!t->match(actual)){
            cerr << "Error: argumento #" << i+1 << " de la función '"
                << stm->nombre << "' debe ser de tipo "
                << t->toString()
                << ", pero se recibió "
                << actual->toString() << "." << endl;
             exit(0);
        }
    }
    
}


//   Expresiones


Type* TypeChecker::visit(BinaryExp* e) {
    Type* left = e->left->accept(this);
    Type* right = e->right->accept(this);
    Type* resulType = new Type();
    switch (e->op) {
        case PLUS_OP: 
        case MINUS_OP: 
        case MUL_OP: 
        case DIV_OP: {
            // Verificar que ambos operandos sean tipos numéricos válidos
            bool leftIsNumeric = (left->match(intType) || left->match(floatType) || left->match(int32Type) 
                    || left->match(float32Type) || left->match(uintType) || left->match(uint32Type));
            bool rightIsNumeric = (right->match(intType) || right->match(floatType) || right->match(int32Type) 
                    || right->match(float32Type) || right->match(uintType) || right->match(uint32Type));
            
            if (!leftIsNumeric || !rightIsNumeric) {
                cerr << "Error: operación aritmética requiere operandos tipo válido." << endl;
                exit(0);
            }
            
            // Permitir operaciones entre tipos compatibles
            // Si los tipos son exactamente iguales, usar ese tipo
            if (left->match(right)) {
                resulType = left;
            }
            // Si uno es float y el otro es int (o variante), resultado es float
            else if ((left->toString() == "float" || left->toString() == "float32") && 
                     (right->toString() == "int" || right->toString() == "int32" || 
                      right->toString() == "uint" || right->toString() == "uint32")) {
                resulType = left;
            }
            else if ((right->toString() == "float" || right->toString() == "float32") && 
                     (left->toString() == "int" || left->toString() == "int32" || 
                      left->toString() == "uint" || left->toString() == "uint32")) {
                resulType = right;
            }
            // Si ambos son int variants o uint variants, usar el de la izquierda
            else {
                resulType = left;
            }
            break;
        }
        case LS_OP:    
        case LSEQ_OP:
        case GR_OP:
        case GREQ_OP:  
        case EQ_OP: {
            // Para comparaciones, permitir tipos numéricos compatibles
            bool leftIsNumeric = (left->match(intType) || left->match(floatType) || left->match(int32Type) 
                    || left->match(float32Type) || left->match(uintType) || left->match(uint32Type));
            bool rightIsNumeric = (right->match(intType) || right->match(floatType) || right->match(int32Type) 
                    || right->match(float32Type) || right->match(uintType) || right->match(uint32Type));
            
            if (!leftIsNumeric || !rightIsNumeric) {
                cerr << "Error: comparación requiere operandos numéricos." << endl;
                exit(0);
            }
            resulType = boolType;
            break;
        }
        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
        
    }
    expressionTypes[e] = resulType;
    return resulType;
}

Type* TypeChecker::visit(NumberExp* e){
    Type* resultType = new Type();
    
    // Si tiene tipo específico del cast
    if (!e->specificType.empty()) {
        if (!resultType->set_basic_type(e->specificType)) {
            cerr << "Error: tipo de literal inválido: " << e->specificType << endl;
            exit(0);
        }
    }
    // Si es float sin tipo específico → float (64-bit por defecto)
    else if (e->isFloat) {
        resultType = floatType;
    }
    // Si es int sin tipo específico → int (64-bit por defecto)
    else {
        resultType = intType;
    }
    
    expressionTypes[e] = resultType;
    return resultType;
}

Type* TypeChecker::visit(BoolExp* e){
    expressionTypes[e] = boolType;
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
    expressionTypes[e] = it->second;
    return it->second;
}



Type* TypeChecker::visit(IdExp* e){
    if(!env.check(e->value)){
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    expressionTypes[e] = env.lookup(e->value);
    return env.lookup(e->value);
}

Type* TypeChecker::visit(IfExp* e){

    Type* ctype = e->condicion->accept(this);
    if(!ctype->match(boolType)){
        cerr << "Error: condición de if-expression debe ser bool."<< endl;
        exit(0);
    }
    //
    int a = locales;

    Type* thentype = e->then->accept(this);
    //
    int b = locales;
    Type* elstype = e->els->accept(this);
    //
    int c = locales;
    if(!thentype->match(elstype)){
        cerr << "Error: las ramas del if-expression tienen tipos incompatibles."<< endl;
        exit(0);
    }
    locales = a + max(b-a,c-b);
    expressionTypes[e] = thentype;
    return thentype;
}

Type* TypeChecker::visit(CastExp* e){
    // Primero verificar el tipo de la expresión interna
    Type* sourceType = e->e->accept(this);
    
    // Crear el tipo de destino
    Type* targetType = new Type();
    if (!targetType->set_basic_type(e->tipo)){
        cerr << "Error: tipo de casteo inválido: " << e->tipo << endl;
        exit(0);
    }
    
    // Verificar que la conversión sea válida (opcional pero recomendado)
    // Por ejemplo, podrías verificar que no se haga cast de tipos incompatibles
    
    // Guardar el tipo resultante (el tipo de destino)
    expressionTypes[e] = targetType;
    
    return targetType;
}