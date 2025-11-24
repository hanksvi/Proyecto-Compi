#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}


int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor)  { return visitor->visit(this); }

int IfExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int CastExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}



int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Body::accept(Visitor* visitor){
    return visitor->visit(this);
}

int VarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FunDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int ReturnStm::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallStm::accept(Visitor* visitor){
    return visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////////

string GenCodeVisitor::obtenerTipoString(Exp* exp) {
    Type* tipo = tipe.getExpressionType(exp);
    return tipo->toString();
}


int GenCodeVisitor::generar(Program* program) {
    
    tipe.typecheck(program);
    fun_reserva = tipe.fun_locales;
    program->accept(this);
    
        return 0;
}

int GenCodeVisitor::visit(Program* program) {
out << ".data\nprint_fmt: .string \"%ld \\n\""<<endl;
    enNivelGlobal = true;
    env.add_level();
    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    for (auto& [var, _] : memoriaGlobal) {
        out << var << ": .quad 0"<<endl;
    }

    out << ".text\n";
    
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    out << ".globl main\n";
    out << "main:\n";
    out << " pushq %rbp\n";
    out << " movq %rsp, %rbp\n";
    enNivelGlobal = false;
    program->cuerpo->accept(this);

    out << " movl $0, %eax\n";
    out << " leave\n";
    out << " ret\n";

    env.remove_level();
    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
        return 0;
}

int GenCodeVisitor::visit(VarDec* vd) {
    for (auto var : vd->vars) {
        if (enNivelGlobal) {
            memoriaGlobal[var] = true;
        } else {
            env.add_var(var, offset);
            offset -= 8;
        }
    }
        return 0;
}



int GenCodeVisitor::visit(NumberExp* exp) {
    out << " movq $" << exp->value << ", %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if(env.check_local(exp->value)){
        out << " movq " << env.lookup(exp->value) << "(%rbp), %rax"<<endl;
    }
    else if (memoriaGlobal.count(exp->value))
        out << " movq " << exp->value << "(%rip), %rax"<<endl;
    else{
        cerr << "Error: variable '" << exp->value << "' no encontrada." << endl;
        exit(1);
    }
    
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    switch (exp->op) {
        case PLUS_OP:  out << " addq %rcx, %rax\n"; break;
        case MINUS_OP: out << " subq %rcx, %rax\n"; break;
        case MUL_OP:   out << " imulq %rcx, %rax\n"; break;
        case DIV_OP:   out << " cqto\n"
                        <<" idivq %rcx\n"; 
                    break;
        case LS_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case LSEQ_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GR_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " seta %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GREQ_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setae %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " sete %al\n"
                      << " movzbq %al, %rax\n";
            break;
    }
    return 0;
}


int GenCodeVisitor::visit(AssignStm* stm) {
    stm->e->accept(this);
    if (memoriaGlobal.count(stm->id))
        out << " movq %rax, " << stm->id << "(%rip)"<<endl;
    else
        out << " movq %rax, " << env.lookup(stm->id) << "(%rbp)"<<endl;
            return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);
    out <<
        " movq %rax, %rsi\n"
        " leaq print_fmt(%rip), %rdi\n"
        " movl $0, %eax\n"
        " call printf@PLT\n";
            return 0;
}

int GenCodeVisitor::visit(FcallStm* stm) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = stm->argumentos.size();
    for (int i = 0; i < size; i++) {
        stm->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }
    out << "call " << stm->nombre << endl;
    return 0;
}


int GenCodeVisitor::visit(Body* body) {
    env.add_level();
    for (auto dec : body->declarations){
        dec->accept(this);
    }
    for (auto s : body->StmList){
        s->accept(this);
    }
    env.remove_level();
        return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
   stm->then->accept(this);
    out << " jmp endif_" << label << endl;
    out << " else_" << label << ":"<< endl;
    if (stm->els) stm->els->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->b->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}



int GenCodeVisitor::visit(ReturnStm* stm) {
    stm->e->accept(this);
    out << " jmp .end_"<<nombreFuncion << endl;
    return 0;
}

int GenCodeVisitor::visit(FunDec* fd) {
    
    entornoFuncion = true;
    
    offset = -8;
    nombreFuncion = fd->nombre;
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << ".globl " << fd->nombre << endl;
    out << fd->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    out << " subq $" << fun_reserva[fd->nombre]*8 << ", %rsp" << endl;
    int size = fd->Pnombres.size();
    for (int i = 0; i < size; i++) {
        env.add_var(fd->Pnombres[i], offset);
        out << " movq " << argRegs[i] << "," << offset << "(%rbp)" << endl;
        offset -= 8;
    }
    for (auto i: fd->cuerpo->declarations){
        i->accept(this);
    }
    int reserva = -offset - 8;

    for (auto i: fd->cuerpo->StmList){
        i->accept(this);
    }
    
    out << ".end_"<< fd->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }
    out << "call " << exp->nombre << endl;
    return 0;
}


int GenCodeVisitor::visit(IfExp* exp){
    
    int label = labelcont++;
    
    // Evaluar condición
    exp->condicion->accept(this);
    
    // Si es falso, saltar al else
    out << " cmpq $0, %rax" << endl;
    out << " je ifexp_else_" << label << endl;
    
    // Rama then
    exp->then->accept(this);
    out << " jmp ifexp_end_" << label << endl;
    
    // Rama else
    out << "ifexp_else_" << label << ":" << endl;
    exp->els->accept(this);
    
    // Fin (el resultado queda en %rax)
    out << "ifexp_end_" << label << ":" << endl;
    
    return 0;
}

int GenCodeVisitor::visit(CastExp* exp){
    
    exp->e->accept(this);

    string origenType = obtenerTipoString(exp->e);
    string destiniType = exp->tipo;

    if(origenType == destiniType){
        return 0;    
    }

    // int -> bool
    if((origenType == "int" || origenType == "int32") && destiniType == "bool"){
        out << " cmpq $0, %rax" << endl;
        out << " movl $0, %eax" << endl;
        out << " setne %al" << endl;
        out << " movzbq %al, %rax" << endl;
        return 0;
    }
    
    // bool -> int
    if (origenType == "bool" && (destiniType == "int" || destiniType == "int32")) {
        
        return 0;
    }

    // int -> float
    if ((origenType == "int" || origenType == "int32") && 
        (destiniType == "float" || destiniType == "float32")) {
        out << " cvtsi2sd %rax, %xmm0" << endl;
        out << " movq %xmm0, %rax" << endl;
        return 0;
    }

    // float -> int
    if ((origenType == "float" || origenType == "float32") && 
        (destiniType == "int" || destiniType == "int32")) {
        out << " movq %rax, %xmm0" << endl;
        out << " cvttsd2si %xmm0, %rax" << endl;
        return 0;
    }
    return 0;
}
int GenCodeVisitor::visit(BoolExp* exp){
    out << " movq $" << (exp->valor ? 1 : 0) << ", %rax" << endl;
    return 0;   
}




//////////////////////////////////////////////////////////
//PrintVisitor
//////////////////////////////////////////////////////////

int PrintVisitor::visit(NumberExp* exp) {
    
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->valor !=0)
        cout<<"true";
    else
        cout<<"false";

    return 0;
}

int PrintVisitor::visit(IdExp* exp) {
    
    cout << exp->value;
    return 0;
}



int PrintVisitor::visit(IfExp* exp){
    cout<<"if ";
    exp->condicion->accept(this);
    cout<<": ";
    exp->then->accept(this);
    cout<<" else: ";
    exp->els->accept(this);
    return 0;
}

int PrintVisitor::visit(CastExp* exp){
    exp->e->accept(this);
    cout<<"."<<exp->tipo; 
    return 0;
}
int PrintVisitor::visit(BinaryExp* exp) {
    
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(WhileStm* stm) {
    indent();
    cout<<"while ";
    stm->condition->accept(this);
    cout<<": "<<endl;
    indentLevel++;
    stm->b->accept(this);
    indentLevel--;
    return 0;
}

int PrintVisitor::visit(IfStm* stm) {
    indent();
    cout<<"if ";
    stm->condition->accept(this);
    cout<<": "<<endl;
    indentLevel++;
    stm->then->accept(this);
    indentLevel--;
    if(stm->els){
        indent();
        cout<<"else: "<<endl;
        indentLevel++;
        stm->els->accept(this);
        indentLevel--;
    }

    return 0;
    
}

int PrintVisitor::visit(FcallStm* stm) {
    cout << stm-> nombre << "(";
    if(stm->argumentos.size()==0){
        cout << ")";
        return 0;
    }
    stm->argumentos[0]->accept(this);
    for (int i= 1; i<stm->argumentos.size(); i++) {
        cout << ",";
        stm->argumentos[i]->accept(this);
        
    }
    cout << ")";
    return 0;
}

int PrintVisitor::visit(PrintStm* stm) {
    indent();
    cout << "echo(";
    stm->e->accept(this);
    cout << ")" << endl;
    return 0;
}

int PrintVisitor::visit(AssignStm* stm) {
    indent();
    cout << stm->id << "=";
    stm->e->accept(this);
    cout << endl;
    return 0;
}

int PrintVisitor::visit(ReturnStm* stm) {
    indent();
    cout << "return (";
    if (stm->e) stm->e->accept(this);
    cout<<")";
    cout << endl;
    return 0;
}

int PrintVisitor::visit(VarDec* vd) {
    indent();
    cout << "var ";
    cout<<vd->vars[0];
    for (int i = 1; i < vd->vars.size(); ++i){
        cout << "," << vd->vars[i];
    }
    cout<<": " << vd->type << endl;

    return 0;
}

int PrintVisitor::visit(Body* b) {
    
    for (auto i : b->declarations){
        i->accept(this);
    }
    for (auto i : b->StmList){
        i->accept(this);
    }

    return 0;
}

int PrintVisitor::visit(FcallExp* fcall) {
    cout << fcall-> nombre << "(";
    if(fcall->argumentos.size()==0){
        cout << ")";
        return 0;
    }
    fcall->argumentos[0]->accept(this);
    for (int i= 1; i<fcall->argumentos.size(); i++) {
        cout << ",";
        fcall->argumentos[i]->accept(this);
        
    }
    cout << ")";
    return 0;
}

int PrintVisitor::visit(FunDec* fd) {
    indent();
    cout << "proc " << fd->nombre << "(";
    for (size_t i = 0; i <  fd->Pnombres.size(); ++i) {
        cout<< fd->Pnombres[i]<<":";
        cout << fd->Ptipos[i];
        if (i < fd->Pnombres.size() - 1)
            cout << ", ";
    }
    cout << "): "<<fd->tipo<<" = "<<endl;
    indentLevel++;
    fd->cuerpo->accept(this);
    indentLevel--;
    return 0;
}

int PrintVisitor::visit(Program* p) {
    indent();
    for( auto vd : p->vdlist){
        vd->accept(this);
    }
    for (auto fd : p->fdlist){
        fd->accept(this);
    }
    p->cuerpo->accept(this);   
    return 0;
}

void PrintVisitor::imprimir(Program* programa) {
    if (programa) {
        programa->accept(this);
    }
}

////////////////////////////////////////////////////////////////
//EVALVisitor
////////////////////////////////////////////////////////////////
unordered_map<string, int> variablesGlobales;
unordered_map<string, int> variablesLocales;

unordered_map<string, FunDec*> tablaDeFunciones;

int valorRetorno = 0;
bool hayRetorno = false;

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}
int EVALVisitor::visit(IdExp* exp) {
    if (variablesLocales.count(exp->value)) {
        return variablesLocales[exp->value];
    } else if (variablesGlobales.count(exp->value)) {
        return variablesGlobales[exp->value];
    } else {
        cerr << "Error: variable '" << exp->value << "' no declarada" << endl;
        return 0;
    }
}

int EVALVisitor::visit(BoolExp* exp) {
    return exp->valor;
}

int EVALVisitor::visit(StringExp* exp) {
    // Para el interprete, las strings no tienen un valor numérico
    // Retornamos 0 por defecto
    return 0;
}


int EVALVisitor::visit(BinaryExp* exp) {
    int izq = exp->left->accept(this);
    int der = exp->right->accept(this);
    
    switch (exp->op) {
        case PLUS_OP:   return izq + der;
        case MINUS_OP:  return izq - der;
        case MUL_OP:    return izq * der;
        case DIV_OP:    
            if (der == 0) {
                cerr << "Error: división por cero" << endl;
                return 0;
            }
            return izq / der;
        case LS_OP:     return izq < der ? 1 : 0;
        case LSEQ_OP:   return izq <= der ? 1 : 0;
        case GR_OP:     return izq > der ? 1 : 0;
        case GREQ_OP:   return izq >= der ? 1 : 0;
        case EQ_OP:     return izq == der ? 1 : 0;
        default:        return 0;
    }
}

int EVALVisitor::visit(IfExp* exp) {
    if(exp->condicion->accept(this)){
        exp->then->accept(this);
    }
    else if(exp->els != nullptr){
        exp->els->accept(this);
    }
    return 0;
};


int EVALVisitor::visit(CastExp* exp) {
    double valor = exp->e->accept(this);
    if(exp->tipo == "int"){
        return (int)valor;
    }
    else if(exp->tipo == "float"){
        return valor;
    }
    else if(exp->tipo == "bool"){
        return (valor !=0) ? 1: 0;
    }
    cerr << "Error: cast a tipo '" << exp->tipo << "' no soportado" << endl;
    return 0;
};


int EVALVisitor::visit(AssignStm* stm) {
    int valor = stm->e->accept(this);
    
    if (variablesLocales.count(stm->id)) {
        variablesLocales[stm->id] = valor;
    } else {
        variablesGlobales[stm->id] = valor;
    }
    
    return 0;
}

int EVALVisitor::visit(PrintStm* stm) {
    int valor = stm->e->accept(this);
    cout << valor << endl;
    return 0;
}
int EVALVisitor::visit(IfStm* stm) {
    int condicion = stm->condition->accept(this);
    
    if (condicion != 0) {
        stm->then->accept(this);
    } 
    else if (stm->els) {
        stm->els->accept(this);
    }
    
    return 0;
}
int EVALVisitor::visit(WhileStm* stm) {
    while (stm->condition->accept(this) != 0) {
        stm->b->accept(this);
        
        if (hayRetorno) break;
    }
    
    return 0;
}
int EVALVisitor::visit(VarDec* vd) {
    for (const auto& var : vd->vars) {
        if (variablesLocales.size() > 0) {
            variablesLocales[var] = 0;
        } else {
            variablesGlobales[var] = 0;
        }
    }
    
    return 0;
}
int EVALVisitor::visit(Body* b) {
    for (auto dec : b->declarations) {
        dec->accept(this);
    }
    
    for (auto stm : b->StmList) {
        stm->accept(this);
        
        if (hayRetorno) break;
    }
    
    return 0;
}
int EVALVisitor::visit(ReturnStm* stm) {
    if (stm->e) {
        valorRetorno = stm->e->accept(this);
    } else {
        valorRetorno = 0;
    }
    
    hayRetorno = true;
    
    return valorRetorno;
}
int EVALVisitor::visit(FunDec* fd) {
    tablaDeFunciones[fd->nombre] = fd;
    return 0;
}
int EVALVisitor::visit(FcallExp* fcall) {
    if (!tablaDeFunciones.count(fcall->nombre)) {
        cerr << "Error: función '" << fcall->nombre << "' no definida" << endl;
        return 0;
    }
    
    FunDec* funcion = tablaDeFunciones[fcall->nombre];
    
    unordered_map<string, int> variablesAnteriores = variablesLocales;
    variablesLocales.clear();
    
    for (size_t i = 0; i < fcall->argumentos.size(); i++) {
        int valorArg = fcall->argumentos[i]->accept(this);
        if (i < funcion->Pnombres.size()) {
            variablesLocales[funcion->Pnombres[i]] = valorArg;
        }
    }
    
    hayRetorno = false;
    funcion->cuerpo->accept(this);
    
    int resultado = valorRetorno;
    
    variablesLocales = variablesAnteriores;
    hayRetorno = false;
    
    return resultado;
}

int EVALVisitor::visit(FcallStm* stm) {
    if (!tablaDeFunciones.count(stm->nombre)) {
        cerr << "Error: función '" << stm->nombre << "' no definida" << endl;
        return 0;
    }
    
    FunDec* funcion = tablaDeFunciones[stm->nombre];
    
    unordered_map<string, int> variablesAnteriores = variablesLocales;
    variablesLocales.clear();
    
    for (size_t i = 0; i < stm->argumentos.size(); i++) {
        int valorArg = stm->argumentos[i]->accept(this);
        if (i < funcion->Pnombres.size()) {
            variablesLocales[funcion->Pnombres[i]] = valorArg;
        }
    }
    
    hayRetorno = false;
    funcion->cuerpo->accept(this);
    
    int resultado = valorRetorno;
    
    variablesLocales = variablesAnteriores;
    hayRetorno = false;
    
    return resultado;
}

int EVALVisitor::visit(Program* p) {
    for (auto vd : p->vdlist) {
        vd->accept(this);
    }
    
    for (auto fd : p->fdlist) {
        fd->accept(this);
    }
    
    p->cuerpo->accept(this);
    
    return 0;
}

int EVALVisitor::visit(IfExp* exp) {
    int condicion = exp->condicion->accept(this);
    if (condicion != 0) {
        return exp->then->accept(this);
    } else {
        return exp->els->accept(this);
    }
}

int EVALVisitor::visit(CastExp* exp) {
    // Para el cast, simplemente evaluamos la expresión base
    // La conversión de tipos podría implementarse aquí
    return exp->e->accept(this);
}

void EVALVisitor::interprete(Program* programa) {
    if (programa) {
        variablesGlobales.clear();
        variablesLocales.clear();
        tablaDeFunciones.clear();
        hayRetorno = false;
        valorRetorno = 0;
        
   
        programa->accept(this);
    }
}

//////////////////////////////////////////////////////////////
//TYPECHECKERVISITOR
//////////////////////////////////////////////////////////////

unordered_map<string, FunDec*> tc_tablaFunciones;
unordered_map<string, string> tc_tablaVariablesGlobales;
unordered_map<string, string> tc_tablaVariablesLocales;
unordered_map<string, int> tc_fun_locales;
unordered_map<string, int> tc_var_locales;
bool tc_hayErrores = false;

int TypeCheckerVisitor::visit(NumberExp* exp) {
    return 0; 
}

int TypeCheckerVisitor::visit(IdExp* exp) {
    if (tc_tablaVariablesLocales.count(exp->value)) {
        return 0; 
    } else if (tc_tablaVariablesGlobales.count(exp->value)) {
        return 0; 
    } else {
        cerr << "Error de tipo: variable '" << exp->value << "' no declarada" << endl;
        tc_hayErrores = true;
        return -1;
    }
}

int TypeCheckerVisitor::visit(BinaryExp* exp) {

    int izq = exp->left->accept(this);
    int der = exp->right->accept(this);
    
    if (izq == -1 || der == -1) {
        return -1; 
    }
    
    return 0; 
}

int TypeCheckerVisitor::visit(AssignStm* stm) {
    if (!tc_tablaVariablesLocales.count(stm->id) && !tc_tablaVariablesGlobales.count(stm->id)) {
        cerr << "Error de tipo: variable '" << stm->id << "' no declarada en asignación" << endl;
        tc_hayErrores = true;
        return -1;
    }
    
    int resultado = stm->e->accept(this);
    
    return resultado;
}

int TypeCheckerVisitor::visit(PrintStm* stm) {
    return stm->e->accept(this);
}

int TypeCheckerVisitor::visit(IfStm* stm) {
    int condicion = stm->condition->accept(this);
    
    int then_result = stm->then->accept(this);
    
    int else_result = 0;
    if (stm->els) {
        else_result = stm->els->accept(this);
    }
    
    if (condicion == -1 || then_result == -1 || else_result == -1) {
        return -1;
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(WhileStm* stm) {
    int condicion = stm->condition->accept(this);
    
    int body_result = stm->b->accept(this);
    
    if (condicion == -1 || body_result == -1) {
        return -1;
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(VarDec* vd) {
    for (const auto& var : vd->vars) {
        if (tc_var_locales.size() > 0 || tc_fun_locales.size() > 0) {
            if (tc_tablaVariablesLocales.count(var)) {
                cerr << "Error de tipo: variable local '" << var << "' ya declarada" << endl;
                tc_hayErrores = true;
            }
            tc_tablaVariablesLocales[var] = vd->type;
        } else {
            if (tc_tablaVariablesGlobales.count(var)) {
                cerr << "Error de tipo: variable global '" << var << "' ya declarada" << endl;
                tc_hayErrores = true;
            }
            tc_tablaVariablesGlobales[var] = vd->type;
        }
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(Body* body) {
    for (auto dec : body->declarations) {
        dec->accept(this);
    }
    
    
    for (auto stm : body->StmList) {
        stm->accept(this);
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(ReturnStm* r) {
    if (r->e) {
        return r->e->accept(this);
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(FunDec* fd) {
    if (tc_tablaFunciones.count(fd->nombre)) {
        cerr << "Error de tipo: función '" << fd->nombre << "' ya declarada" << endl;
        tc_hayErrores = true;
        return -1;
    }
    
    tc_tablaFunciones[fd->nombre] = fd;
    tc_fun_locales[fd->nombre] = 1;
    
    unordered_map<string, string> variablesAnteriores = tc_tablaVariablesLocales;
    tc_tablaVariablesLocales.clear();
    
    for (size_t i = 0; i < fd->Pnombres.size(); i++) {
        tc_tablaVariablesLocales[fd->Pnombres[i]] = fd->Ptipos[i];
        tc_var_locales[fd->Pnombres[i]] = 1;
    }
    
    fd->cuerpo->accept(this);
    
    tc_tablaVariablesLocales = variablesAnteriores;
    tc_var_locales.clear();
    
    return 0;
}

int TypeCheckerVisitor::visit(FcallExp* fcall) {
    if (!tc_tablaFunciones.count(fcall->nombre)) {
        cerr << "Error de tipo: función '" << fcall->nombre << "' no declarada" << endl;
        tc_hayErrores = true;
        return -1;
    }
    
    FunDec* funcion = tc_tablaFunciones[fcall->nombre];
    
    if (fcall->argumentos.size() != funcion->Pnombres.size()) {
        cerr << "Error de tipo: función '" << fcall->nombre << "' espera " 
             << funcion->Pnombres.size() << " argumentos, pero se proporcionaron " 
             << fcall->argumentos.size() << endl;
        tc_hayErrores = true;
        return -1;
    }
    
    for (auto arg : fcall->argumentos) {
        int resultado = arg->accept(this);
        if (resultado == -1) {
            return -1;
        }
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(Program* p) {
    for (auto vd : p->vdlist) {
        vd->accept(this);
    }
    
    for (auto fd : p->fdlist) {
        fd->accept(this);
    }
    
    p->cuerpo->accept(this);
    
    return 0;
}

int TypeCheckerVisitor::visit(StringExp* exp) {
    return 0; // Las strings son válidas
}

int TypeCheckerVisitor::visit(IfExp* exp) {
    int condicion = exp->condicion->accept(this);
    int then_result = exp->then->accept(this);
    int else_result = exp->els->accept(this);
    
    if (condicion == -1 || then_result == -1 || else_result == -1) {
        return -1;
    }
    
    return 0;
}

int TypeCheckerVisitor::visit(CastExp* exp) {
    // Verificar que la expresión base sea válida
    int resultado = exp->e->accept(this);
    
    // Aquí podríamos agregar validación de tipos de cast
    // Por ahora solo verificamos que la expresión sea válida
    return resultado;
}

void TypeCheckerVisitor::check(Program* program) {
    if (program) {
        tc_tablaFunciones.clear();
        tc_tablaVariablesGlobales.clear();
        tc_tablaVariablesLocales.clear();
        tc_fun_locales.clear();
        tc_var_locales.clear();
        tc_hayErrores = false;
        
        cout << "\n Verificación de tipos" << endl;
        
        program->accept(this);
        
        if (!tc_hayErrores) {
            cout << "No se encontraron errores de tipos" << endl;
        } else {
            cout << "Se encontraron errores de tipos" << endl;
        }
    }
}
