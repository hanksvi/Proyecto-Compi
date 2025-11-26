#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <cstdint>
#include <iomanip>
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

bool GenCodeVisitor::isFloatType(const string& type) {
    return type == "float" || type == "float32";
}

bool GenCodeVisitor::isFloat64(const string& type) {
    return type == "float";  // 64 bits
}

bool GenCodeVisitor::isFloat32(const string& type) {
    return type == "float32";  // 32 bits
}

bool GenCodeVisitor::isInt64(const string& type) {
    return type == "int" || type == "uint";
}

bool GenCodeVisitor::isInt32(const string& type) {
    return type == "int32" || type == "uint32";
}

bool GenCodeVisitor::isUnsignedType(const string& type) {
    return type == "uint" || type == "uint32";
}

string GenCodeVisitor::obtenerTipoString(Exp* exp) {
    Type* tipo = tipe.getExpressionType(exp);
    return tipo->toString();
}

int GenCodeVisitor::getSizeOfType(const string& typeStr) {
    // Retorna el tamaño en bytes de un tipo
    if (typeStr == "int" || typeStr == "float" || typeStr == "uint" || typeStr == "bool") {
        return 8; // 64 bits
    } else if (typeStr == "int32" || typeStr == "float32" || typeStr == "uint32") {
        return 4; // 32 bits
    } else {
        return 8; // Por defecto 8 bytes
    }
}


int GenCodeVisitor::generar(Program* program) {
    
    tipe.typecheck(program);
    fun_reserva = tipe.fun_locales;
    program->accept(this);
    
        return 0;
}

int GenCodeVisitor::visit(Program* program) {
    out << ".data\n";
     // Formatos de impresión
    out << "print_fmt: .string \"%ld\\n\"\n";           // int/int64
    out << "print_fmt_uint: .string \"%lu\\n\"\n";      // uint/uint64
    out << "print_fmt_float: .string \"%f\\n\"\n";      // float/float32 (ambos usan %f)
    out << "print_fmt_true: .string \"true\\n\"\n";     // bool true
    out << "print_fmt_false: .string \"false\\n\"\n";   // bool false
    enNivelGlobal = true;
    env.add_level();
    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    for (auto& [var, _] : memoriaGlobal) {
        string varType = "";
        for(auto vd: program->vdlist){
            for(auto v: vd->vars){
                if (v == var){
                    varType = vd->type;
                    break;
                }
            }
        }
        if (isInt32(varType) || isFloat32(varType)){
            out << var << ": .long 0"<<endl; // 32 bits
        } else {
            out << var << ": .quad 0\n"; // 64 bits
        }
        
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
 // Emitir constantes float al final
    if (!floatConstants.empty()) {
        out << ".section .rodata\n";
        out << ".align 8\n";
        for (auto& [label, valueAndType] : floatConstants) {
            double value = valueAndType.first;
            string tipo = valueAndType.second;
            
            out << label << ":\n";
            if (isFloat32(tipo)) {
                out << " .float " << value << "\n";
            } else {
                out << " .double " << value << "\n";
            }
        }
    }
    
    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    return 0;
}

int GenCodeVisitor::visit(VarDec* vd) {
    int sizeOfType = getSizeOfType(vd->type);
    for (auto var : vd->vars) {
        if (enNivelGlobal) {
            memoriaGlobal[var] = true;
             globalVariableTypes[var] = vd->type; 
        } else {
            variableTypes[var] = vd->type;
            env.add_var(var, offset);
            offset -= sizeOfType;
        }
    }
        return 0;
}



int GenCodeVisitor::visit(NumberExp* exp) {
    string tipo = obtenerTipoString(exp);
    
    if (isFloatType(tipo)) {
        // Para literales float, crear una constante en .data
        string labelName = "float_const_" + to_string(floatConstCounter++);
        
        // Guardar constante con su tipo
        floatConstants[labelName] = make_pair(exp->value, tipo);
        
        // Cargar desde memoria
        if (isFloat64(tipo)) {
            out << " movsd " << labelName << "(%rip), %xmm0\n";
            out << " movq %xmm0, %rax\n";
        } else {
            out << " movss " << labelName << "(%rip), %xmm0\n";
            out << " movd %xmm0, %eax\n";
            out << " movl %eax, %eax\n";
        }
    } else {
        // Para enteros, carga directa
        out << " movq $" << exp->value << ", %rax\n";
    }
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    string varType = "";
    
    // Primero intentar buscar como variable local/parámetro (en cualquier nivel del ambiente)
    if(env.check(exp->value)){
        // Variable local o parámetro
        if(variableTypes.count(exp->value)){
            varType = variableTypes[exp->value];
        }
        
        if(isInt32(varType) || isFloat32(varType) || varType == "uint32"){
            out << " movl "<<env.lookup(exp->value)<< "(%rbp), %eax\n";
            // En x86-64, movl automáticamente hace zero-extend a %rax
        } else{
            out << " movq " << env.lookup(exp->value) << "(%rbp), %rax\n";
        }
    }
    else if (memoriaGlobal.count(exp->value)) {
        // Variable global - usar globalVariableTypes
        if(globalVariableTypes.count(exp->value)){
            varType = globalVariableTypes[exp->value];
        }
        
        if(isInt32(varType) || isFloat32(varType) || varType == "uint32"){
            out << " movl "<< exp->value << "(%rip), %eax\n";
            // En x86-64, movl automáticamente hace zero-extend a %rax
        } else{
            out << " movq " << exp->value << "(%rip), %rax\n";
        }
    }
    else{
        cerr << "Error: variable '" << exp->value << "' no encontrada." << endl;
        exit(1);
    }
    
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {

    if (exp->isConstant) {
        string tipo = obtenerTipoString(exp);
        
        if (isFloatType(tipo)) {
            string labelName = "const_" + to_string(floatConstCounter++);
            floatConstants[labelName] = make_pair(exp->constantValue, tipo);
            
            if (isFloat64(tipo)) {
                out << " movsd " << labelName << "(%rip), %xmm0\n";
                out << " movq %xmm0, %rax\n";
            } else {
                out << " movss " << labelName << "(%rip), %xmm0\n";
                out << " movd %xmm0, %eax\n";
            }
        } else {
            out << " movq $" << (int)exp->constantValue << ", %rax\n";
        }
        return 0;
    }
    string leftType = obtenerTipoString(exp->left);
    string rightType = obtenerTipoString(exp->right);
    bool issFloat = isFloatType(leftType);
    bool issFloat64 = isFloat64(leftType);
    bool issInt64 = isInt64(leftType);
    bool issUnsigned = isUnsignedType(leftType);

    if(issFloat){
        // Operaciones float
        exp->left->accept(this);
        out<<"movq %rax, %xmm0\n";
        out<<"pushq %rax\n";
        exp->right->accept(this);
        out<<"movq %rax, %xmm1\n";
        out<<"pop %rax\n";
        out<<"movq %rax, %xmm0\n";
        switch (exp->op) {
        case PLUS_OP:  
            if(issFloat64)   out << " addsd %xmm1, %xmm0\n";   // 64 bits - double
            else            out << " addss %xmm1, %xmm0\n";            // 32 bits - float
            break;
        case MINUS_OP: 
            if(issFloat64)   out << " subsd %xmm1, %xmm0\n";   // 64 bits - double
            else            out << " subss %xmm1, %xmm0\n";            // 32 bits - float
            break;
        case MUL_OP:   
            if(issFloat64)   out << " mulsd %xmm1, %xmm0\n";   // 64 bits - double
            else            out << " mulss %xmm1, %xmm0\n";            // 32 bits - float
            break;
        case DIV_OP:  
            if(issFloat64)   out << " divsd %xmm1, %xmm0\n";   // 64 bits - double
            else            out << " divss %xmm1, %xmm0\n";            // 32 bits - float
            break;
        case LS_OP:
        case LSEQ_OP:
        case GR_OP:
        case GREQ_OP:
        case EQ_OP:
            if(issFloat64)   out << " ucomisd %xmm1, %xmm0\n";
            else            out << " ucomiss %xmm1, %xmm0\n";
            out << " movl $0, %eax\n";
            if(exp->op == LS_OP)        out<< "setb %al\n";
            else if(exp->op == LSEQ_OP) out<< "setbe %al\n";
            else if(exp->op == GR_OP)   out<< "seta %al\n";
            else if(exp->op == GREQ_OP) out<< "setae %al\n";
            else if(exp->op == EQ_OP)   out<< "sete %al\n";
            out<< " movzbq %al, %rax\n";
            return 0;
          }
    out << " movq %xmm0, %rax\n";
    }
    else {
        exp->left->accept(this);
        out << " pushq %rax\n";
        exp->right->accept(this);
        out << " movq %rax, %rcx\n popq %rax\n";
        switch (exp->op){
        case PLUS_OP:  
            if(issInt64)   out << " addq %rcx, %rax\n";   // 64 bits 
            else            out << " addl %ecx, %eax\n";            // 32 bits
            break;
        case MINUS_OP: 
            if(issFloat64)   out << " subq %rcx, %rax\n";   // 64 bits - double
            else            out << " subl %ecx, %eax\n";            // 32 bits - float
            break;
        case MUL_OP:   
            if (issUnsigned){
                if(issFloat64)   out << " mulq %rcx\n";   // unsingned - 64 bits 
                else            out << " mull %ecx\n";            // unsingned - 32 bits 
            }
            else {
                if(issFloat64)   out << " imulq %rcx\n";   // signed - 64 bits 
                else            out << " imull %ecx\n";            // signed 32 bits 
            }
            
            break;
        case DIV_OP:  
            if(issUnsigned){
                if(issInt64){
                    out << " xorq %rdx, %rdx\n";        // limpiar rdx
                    out << " divq %rcx\n";              // unsigned 64 bits
                }
                else {
                    out << " xorl %edx, %edx\n";        // limpiar edx
                    out << " divl %ecx\n";              // unsigned 32 bits
                }
            } else {
                if(issInt64) {
                    out << " cqto\n";                   // extender signo 64-bits
                    out << " idivq %rcx\n";             // signed 64-bits
                } else {
                    out << " cltd\n";                   // extender signo 32-bits
                    out << " idivl %ecx\n";              // signed 32-bits
                }
            }
            break;
        case LS_OP:
        case LSEQ_OP:
        case GR_OP:
        case GREQ_OP:
            if(issInt64)     out << " cmpq %rcx, %rax\n";
            else            out << " cmpq %ecx, %eax\n";

            out << " movl $0, %eax\n";
            if(issUnsigned){
                // Comparaciones unsigned
                if (exp->op == LS_OP)       out << " setb %al\n";
                else if (exp->op == LSEQ_OP) out << " setbe %al\n";
                else if (exp->op == GR_OP)  out << " seta %al\n";
                else if (exp->op == GREQ_OP) out << " setae %al\n";
            } else {
                // Comparaciones signed
                if (exp->op == LS_OP)       out << " setl %al\n";
                else if (exp->op == LSEQ_OP) out << " setle %al\n";
                else if (exp->op == GR_OP)  out << " setg %al\n";
                else if (exp->op == GREQ_OP) out << " setge %al\n";
            }
            out << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            if (issInt64)    out << " cmpq %rcx, %rax\n";
            else            out << " cmpl %ecx, %eax\n";

            out<< " movl $0, %eax\n";
            out<< " sete %al\n";
            out<< " movzbq %al, %rax\n";
            break;
        }
    }
    return 0;
}


int GenCodeVisitor::visit(AssignStm* stm) {
    string varType = "";
    
    // Buscar tipo en el mapa correcto
    if (env.check_local(stm->id) && variableTypes.count(stm->id)){
        varType = variableTypes[stm->id];
    } else if (memoriaGlobal.count(stm->id) && globalVariableTypes.count(stm->id)){
        varType = globalVariableTypes[stm->id];
    }

    stm->e->accept(this);


    if (memoriaGlobal.count(stm->id)) {
        if (isInt32(varType) || isFloat32(varType) || varType == "uint32") {
            out << " movl %eax, " << stm->id << "(%rip)\n";
        } else {
            out << " movq %rax, " << stm->id << "(%rip)\n";
        }
    } else {
        if (isInt32(varType) || isFloat32(varType) || varType == "uint32") {
            out << " movl %eax, " << env.lookup(stm->id) << "(%rbp)\n";
        } else {
            out << " movq %rax, " << env.lookup(stm->id) << "(%rbp)\n";
        }
    }
    
    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    string exprType = obtenerTipoString(stm->e);
    
    stm->e->accept(this);
    
    if (exprType == "float") {
        // Float 64 bits (double)
        out << " movq %rax, %xmm0\n";
        out << " movq $1, %rax\n";
        out << " leaq print_fmt_float(%rip), %rdi\n";
        out << " call printf@PLT\n";
        
    } else if (exprType == "float32") {
        // Float 32 bits
        out << " movd %eax, %xmm0\n";
        out << " cvtss2sd %xmm0, %xmm0\n";
        out << " movq $1, %rax\n";
        out << " leaq print_fmt_float(%rip), %rdi\n";
        out << " call printf@PLT\n";
        
    } else if (exprType == "uint32") {
        // uint32 - movl ya hizo zero-extend automáticamente
        out << " movq %rax, %rsi\n";
        out << " leaq print_fmt_uint(%rip), %rdi\n";
        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";
        
    } else if (exprType == "uint") {
        // uint64
        out << " movq %rax, %rsi\n";
        out << " leaq print_fmt_uint(%rip), %rdi\n";
        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";
        
    } else if (exprType == "bool") {
        // Bool - imprimir 0 o 1
        out << " movzbq %al, %rax\n";
        out << " movq %rax, %rsi\n";
        out << " leaq print_fmt(%rip), %rdi\n";
        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";

    } else {
        // Int o int32 (con signo)
        if (exprType == "int32") {
            out << " movslq %eax, %rax\n";  // sign-extend 32→64
        }
        out << " movq %rax, %rsi\n";
        out << " leaq print_fmt(%rip), %rdi\n";
        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";
    }
    
    return 0;
}

int GenCodeVisitor::visit(FcallStm* stm) {
    vector<std::string> argRegs64 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    vector<std::string> argRegs32 = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    
    FunDec* fd = nullptr;
    for (auto& [nombre, dec] : tipe.functionDeclarations) {
        if (nombre == stm->nombre) {
            fd = dec;
            break;
        }
    }
    
    int size = stm->argumentos.size();
    for (int i = 0; i < size; i++) {
        stm->argumentos[i]->accept(this);
        
        if (fd && i < fd->Ptipos.size()) {
            if (isInt32(fd->Ptipos[i]) || isFloat32(fd->Ptipos[i])) {
                out << " movl %eax, " << argRegs32[i] << endl;
            } else {
                out << " movq %rax, " << argRegs64[i] << endl;
            }
        } else {
            out << " movq %rax, " << argRegs64[i] << endl;
        }
    }
    
    out << "call " << stm->nombre << endl;
    return 0;
}


int GenCodeVisitor::visit(Body* body) {
    if (!entornoFuncion) {
        env.add_level();
    }
    for (auto dec : body->declarations){
        dec->accept(this);
    }
    for (auto s : body->StmList){
        s->accept(this);
    }
    if (!entornoFuncion) {
        env.remove_level();
    }
        return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    // OPTIMIZACIÓN: Si la condición es constante, no generar código condicional
    if(stm->condition->isConstant){
        if(stm->condition->constantValue != 0){
            stm->then->accept(this);
        }
        else {
            if(stm->els){
                stm->els->accept(this);
            }
        }
        return 0;
    }
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
    // OPTIMIZACIÓN 1: Si la condición es constante y falsa, no generar nada
   if (stm->condition->isConstant && stm->condition->constantValue == 0) {
        return 0;
    }
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
    variableTypes.clear();
    
    offset = -8;
    nombreFuncion = fd->nombre;
    vector<std::string> argRegs64 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    vector<std::string> argRegs32 = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    
    out << ".globl " << fd->nombre << endl;
    out << fd->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    out << " subq $" << fun_reserva[fd->nombre]*8 << ", %rsp" << endl;
    
    env.add_level();
    
    bool wasGlobal = enNivelGlobal;
    enNivelGlobal = false;

    int size = fd->Pnombres.size();
    for (int i = 0; i < size; i++) {
        variableTypes[fd->Pnombres[i]] = fd->Ptipos[i];
        int paramSize = getSizeOfType(fd->Ptipos[i]);
        env.add_var(fd->Pnombres[i], offset);
        
        // Guardar según el tamaño del parámetro
        if (isInt32(fd->Ptipos[i]) || isFloat32(fd->Ptipos[i])) {
            out << " movl " << argRegs32[i] << "," << offset << "(%rbp)" << endl;
        } else {
            out << " movq " << argRegs64[i] << "," << offset << "(%rbp)" << endl;
        }
        
        offset -= paramSize;
    }
    
    for (auto i: fd->cuerpo->declarations){
        i->accept(this);
    }

    for (auto i: fd->cuerpo->StmList){
        i->accept(this);
    }
    
    enNivelGlobal = wasGlobal;
    env.remove_level();
    
    out << ".end_"<< fd->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs64 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    vector<std::string> argRegs32 = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    
    FunDec* fd = nullptr;
    for (auto& [nombre, dec] : tipe.functionDeclarations) {
        if (nombre == exp->nombre) {
            fd = dec;
            break;
        }
    }
    
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        
        if (fd && i < fd->Ptipos.size()) {
            if (isInt32(fd->Ptipos[i]) || isFloat32(fd->Ptipos[i])) {
                out << " movl %eax, " << argRegs32[i] << endl;
            } else {
                out << " movq %rax, " << argRegs64[i] << endl;
            }
        } else {
            out << " movq %rax, " << argRegs64[i] << endl;
        }
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

    
    // int64 → float64 (SIGNED)
    if (origenType == "int" && destiniType == "float") {
        out << " cvtsi2sdq %rax, %xmm0\n";  // signed int64 → double
        out << " movq %xmm0, %rax\n";
        return 0;
    }
    
    // uint64 → float64 (UNSIGNED) - REQUIERE MANEJO ESPECIAL
    if (origenType == "uint" && destiniType == "float") {

        out << " cvtsi2sdq %rax, %xmm0\n";  // Por ahora simple
        out << " movq %xmm0, %rax\n";
        return 0;
    }
    
    // int32 → float32 (SIGNED)
    if (origenType == "int32" && destiniType == "float32") {
        out << " cvtsi2ss %eax, %xmm0\n";  // signed int32 → float
        out << " movd %xmm0, %eax\n";
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // uint32 → float32 (UNSIGNED) - FIX CRÍTICO
    if (origenType == "uint32" && destiniType == "float32") {
        out << " movl %eax, %eax\n";  
        out << " cvtsi2ssq %rax, %xmm0\n";  
        out << " movd %xmm0, %eax\n";
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // int64 → float32 (SIGNED)
    if (origenType == "int" && destiniType == "float32") {
        out << " cvtsi2ssq %rax, %xmm0\n";
        out << " movd %xmm0, %eax\n";
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // uint64 → float32 (UNSIGNED)
    if (origenType == "uint" && destiniType == "float32") {
        out << " cvtsi2ssq %rax, %xmm0\n";
        out << " movd %xmm0, %eax\n";
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // int32 → float64 (SIGNED)
    if (origenType == "int32" && destiniType == "float") {
        out << " cvtsi2sd %eax, %xmm0\n";
        out << " movq %xmm0, %rax\n";
        return 0;
    }
    
    // uint32 → float64 (UNSIGNED) - FIX CRÍTICO
    if (origenType == "uint32" && destiniType == "float") {
        // Zero-extend a 64 bits primero
        out << " movl %eax, %eax\n";  // zero-extend uint32 → uint64
        out << " cvtsi2sdq %rax, %xmm0\n";  // convertir como int64 (ahora es positivo)
        out << " movq %xmm0, %rax\n";
        return 0;
    }

    
    // float64 → int64 (SIGNED)
    if (origenType == "float" && destiniType == "int") {
        out << " movq %rax, %xmm0\n";
        out << " cvttsd2siq %xmm0, %rax\n";
        return 0;
    }
    
    // float64 → uint64 (UNSIGNED)
    if (origenType == "float" && destiniType == "uint") {
        out << " movq %rax, %xmm0\n";
        out << " cvttsd2siq %xmm0, %rax\n";  // truncar a int64
        return 0;
    }
    
    // float32 → int32 (SIGNED)
    if (origenType == "float32" && destiniType == "int32") {
        out << " movd %eax, %xmm0\n";
        out << " cvttss2si %xmm0, %eax\n";
        return 0;
    }
    
    // float32 → uint32 (UNSIGNED)
    if (origenType == "float32" && destiniType == "uint32") {
        out << " movd %eax, %xmm0\n";
        out << " cvttss2si %xmm0, %eax\n";
        return 0;
    }
    
    // float64 → int32 (SIGNED)
    if (origenType == "float" && destiniType == "int32") {
        out << " movq %rax, %xmm0\n";
        out << " cvttsd2si %xmm0, %eax\n";
        return 0;
    }
    
    // float64 → uint32 (UNSIGNED)
    if (origenType == "float" && destiniType == "uint32") {
        out << " movq %rax, %xmm0\n";
        out << " cvttsd2si %xmm0, %eax\n";
        return 0;
    }
    
    // float32 → int64 (SIGNED)
    if (origenType == "float32" && destiniType == "int") {
        out << " movd %eax, %xmm0\n";
        out << " cvttss2siq %xmm0, %rax\n";
        return 0;
    }
    
    // float32 → uint64 (UNSIGNED)
    if (origenType == "float32" && destiniType == "uint") {
        out << " movd %eax, %xmm0\n";
        out << " cvttss2siq %xmm0, %rax\n";
        return 0;
    }

    
    // float32 → float64
    if (origenType == "float32" && destiniType == "float") {
        out << " movd %eax, %xmm0\n";
        out << " cvtss2sd %xmm0, %xmm0\n";
        out << " movq %xmm0, %rax\n";
        return 0;
    }
    
    // float64 → float32
    if (origenType == "float" && destiniType == "float32") {
        out << " movq %rax, %xmm0\n";
        out << " cvtsd2ss %xmm0, %xmm0\n";
        out << " movd %xmm0, %eax\n";
        out << " movl %eax, %eax\n";
        return 0;
    }

    
    // int32 → int64 (con extensión de signo)
    if (origenType == "int32" && destiniType == "int") {
        out << " movslq %eax, %rax\n";
        return 0;
    }
    
    // uint32 → uint64 (con extensión de cero)
    if (origenType == "uint32" && destiniType == "uint") {
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // int32 → uint64 (sign extend primero)
    if (origenType == "int32" && destiniType == "uint") {
        out << " movslq %eax, %rax\n";
        return 0;
    }
    
    // uint32 → int64 (zero extend)
    if (origenType == "uint32" && destiniType == "int") {
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // int64 → int32 (truncar)
    if (origenType == "int" && destiniType == "int32") {
        // Ya está en %eax (32 bits bajos)
        return 0;
    }
    
    // uint64 → uint32 (truncar)
    if (origenType == "uint" && destiniType == "uint32") {
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // int64 → uint32 (truncar)
    if (origenType == "int" && destiniType == "uint32") {
        out << " movl %eax, %eax\n";
        return 0;
    }
    
    // uint64 → int32 (truncar)
    if (origenType == "uint" && destiniType == "int32") {
        // Ya está en %eax
        return 0;
    }

    
    if ((origenType == "int" && destiniType == "uint") ||
        (origenType == "uint" && destiniType == "int") ||
        (origenType == "int32" && destiniType == "uint32") ||
        (origenType == "uint32" && destiniType == "int32")) {
        // Reinterpretación de bits, no hacer nada
        return 0;
    }

    
    if (destiniType == "bool") {
        if (isFloatType(origenType)) {
            if (origenType == "float") {
                out << " movq %rax, %xmm0\n";
                out << " xorpd %xmm1, %xmm1\n";
                out << " ucomisd %xmm1, %xmm0\n";
            } else {
                out << " movd %eax, %xmm0\n";
                out << " xorps %xmm1, %xmm1\n";
                out << " ucomiss %xmm1, %xmm0\n";
            }
            out << " movl $0, %eax\n";
            out << " setne %al\n";
            out << " movzbq %al, %rax\n";
        } else {
            if (origenType == "int" || origenType == "uint") {
                out << " cmpq $0, %rax\n";
            } else {
                out << " cmpl $0, %eax\n";
            }
            out << " movl $0, %eax\n";
            out << " setne %al\n";
            out << " movzbq %al, %rax\n";
        }
        return 0;
    }
    
    // bool → cualquier tipo entero
    if (origenType == "bool" && !isFloatType(destiniType)) {
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
unordered_map<string, double> variablesGlobales;
unordered_map<string, double> variablesLocales;
unordered_map<string, string> tiposVariablesGlobales;   // Tipos de variables globales
unordered_map<string, string> tiposVariablesLocales;    // Tipos de variables locales

unordered_map<string, FunDec*> tablaDeFunciones;

double valorRetorno = 0;
double ultimoValorFloat = 0;  // Almacenar último valor float
string ultimoTipo = "int";    // Almacenar tipo de última expresión
bool hayRetorno = false;
int scopeDepth = 0;  

int EVALVisitor::visit(NumberExp* exp) {
    ultimoValorFloat = exp->value;
    // Si tiene punto decimal o fue marcado como float, es float
    ultimoTipo = exp->isFloat ? "float" : "int";
    return (int)exp->value;
}
int EVALVisitor::visit(IdExp* exp) {
    double val = 0;
    string tipo = "int";
    if (variablesLocales.count(exp->value)) {
        val = variablesLocales[exp->value];
        tipo = tiposVariablesLocales[exp->value];
    } else if (variablesGlobales.count(exp->value)) {
        val = variablesGlobales[exp->value];
        tipo = tiposVariablesGlobales[exp->value];
    } else {
        cerr << "Error: variable '" << exp->value << "' no declarada" << endl;
        return 0;
    }
    ultimoValorFloat = val;
    ultimoTipo = tipo;
    return (int)val;
}

int EVALVisitor::visit(BoolExp* exp) {
    return exp->valor;
}


int EVALVisitor::visit(BinaryExp* exp) {
    double izq = ultimoValorFloat;  // Obtener valor float del izquierdo
    exp->left->accept(this);
    double izqVal = ultimoValorFloat;
    string izqTipo = ultimoTipo;
    
    double der = ultimoValorFloat;  // Obtener valor float del derecho
    exp->right->accept(this);
    double derVal = ultimoValorFloat;
    string derTipo = ultimoTipo;
    
    double resultado = 0;
    // Determinar tipo del resultado
    if (izqTipo == "float" || izqTipo == "float32" || derTipo == "float" || derTipo == "float32") {
        ultimoTipo = "float";
    } else if (exp->op >= LS_OP && exp->op <= EQ_OP) {
        // Comparaciones siempre dan bool, pero lo representamos como int
        ultimoTipo = "bool";
    } else {
        ultimoTipo = "int";
    }
    
    switch (exp->op) {
        case PLUS_OP:   resultado = izqVal + derVal; break;
        case MINUS_OP:  resultado = izqVal - derVal; break;
        case MUL_OP:    resultado = izqVal * derVal; break;
        case DIV_OP:    
            if (derVal == 0) {
                cerr << "Error: división por cero" << endl;
                resultado = 0;
            } else {
                resultado = izqVal / derVal;
            }
            break;
        case LS_OP:     resultado = izqVal < derVal ? 1 : 0; break;
        case LSEQ_OP:   resultado = izqVal <= derVal ? 1 : 0; break;
        case GR_OP:     resultado = izqVal > derVal ? 1 : 0; break;
        case GREQ_OP:   resultado = izqVal >= derVal ? 1 : 0; break;
        case EQ_OP:     resultado = izqVal == derVal ? 1 : 0; break;
        default:        resultado = 0;
    }
    ultimoValorFloat = resultado;
    return (int)resultado;
}

int EVALVisitor::visit(IfExp* exp) {
    int cond = exp->condicion->accept(this);
    if(cond){
        exp->then->accept(this);
        return 1;
    }
    else if(exp->els != nullptr){
        exp->els->accept(this);
        return 1;
    }
    return 0;
};


int EVALVisitor::visit(CastExp* exp) {
    exp->e->accept(this);
    double valor = ultimoValorFloat;
    if(exp->tipo == "int"){
        ultimoTipo = "int";
        ultimoValorFloat = (int)valor;
        return (int)valor;
    }
    else if(exp->tipo == "int32"){
        ultimoTipo = "int";
        ultimoValorFloat = (int32_t)valor;
        return (int32_t)valor;
    }
    else if(exp->tipo == "uint"){
        ultimoTipo = "uint";
        ultimoValorFloat = (unsigned int)valor;
        return (unsigned int)valor;
    }
    else if(exp->tipo == "uint32"){
        ultimoTipo = "uint";
        ultimoValorFloat = (uint32_t)valor;
        return (uint32_t)valor;
    }
    else if(exp->tipo == "float" || exp->tipo == "float32"){
        ultimoTipo = "float";
        ultimoValorFloat = valor;
        return (int)valor;
    }
    else if(exp->tipo == "bool"){
        ultimoTipo = "bool";
        ultimoValorFloat = (valor !=0) ? 1: 0;
        return (valor !=0) ? 1: 0;
    }
    cerr << "Error: cast a tipo '" << exp->tipo << "' no soportado" << endl;
    return 0;
};


int EVALVisitor::visit(AssignStm* stm) {
    stm->e->accept(this);
    double valor = ultimoValorFloat;
    
    if (variablesLocales.count(stm->id)) {
        variablesLocales[stm->id] = valor;
    } else {
        variablesGlobales[stm->id] = valor;
    }
    
    return 0;
}

int EVALVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);

    if (ultimoTipo == "float" || ultimoTipo == "float32") {
        cout << fixed << setprecision(6) << ultimoValorFloat << endl;
    } else {
        cout << (long long)ultimoValorFloat << endl;
    }
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
        if (scopeDepth > 0) {

            variablesLocales[var] = 0;
            tiposVariablesLocales[var] = vd->type;
        } else {

            variablesGlobales[var] = 0;
            tiposVariablesGlobales[var] = vd->type;
        }
    }
    
    return 0;
}
int EVALVisitor::visit(Body* b) {

    unordered_map<string, double> savedLocals = variablesLocales;
    

    scopeDepth++;
    
    for (auto dec : b->declarations) {
        dec->accept(this);
    }
    
    for (auto stm : b->StmList) {
        stm->accept(this);
        
        if (hayRetorno) break;
    }
    
 
    scopeDepth--;
    variablesLocales = savedLocals;
    
    return 0;
}
int EVALVisitor::visit(ReturnStm* stm) {
    if (stm->e) {

        stm->e->accept(this);

        valorRetorno = ultimoValorFloat;
    } else {
        valorRetorno = 0.0;
        ultimoValorFloat = 0.0;
        ultimoTipo = "int";
    }
    
    hayRetorno = true;
    
    return (int)valorRetorno;  
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
    

    vector<double> valoresArgs;
    for (size_t i = 0; i < fcall->argumentos.size(); i++) {

        fcall->argumentos[i]->accept(this);

        valoresArgs.push_back(ultimoValorFloat);
    }
    

    unordered_map<string, double> variablesAnteriores = variablesLocales;
    unordered_map<string, string> tiposAnteriores = tiposVariablesLocales;
    variablesLocales.clear();
    tiposVariablesLocales.clear();
    
    for (size_t i = 0; i < valoresArgs.size(); i++) {
        if (i < funcion->Pnombres.size()) {
            variablesLocales[funcion->Pnombres[i]] = valoresArgs[i];

        }
    }

    scopeDepth++;
    hayRetorno = false;
    funcion->cuerpo->accept(this);
    scopeDepth--;
    
    double resultado = valorRetorno;

    variablesLocales = variablesAnteriores;
    tiposVariablesLocales = tiposAnteriores;
    hayRetorno = false;
    

    ultimoValorFloat = resultado;

    ultimoTipo = "float";  
    
    return (int)resultado;  
}

int EVALVisitor::visit(FcallStm* stm) {
    if (!tablaDeFunciones.count(stm->nombre)) {
        cerr << "Error: función '" << stm->nombre << "' no definida" << endl;
        return 0;
    }
    
    FunDec* funcion = tablaDeFunciones[stm->nombre];
    
    // Evaluar argumentos
    vector<double> valoresArgs;
    for (size_t i = 0; i < stm->argumentos.size(); i++) {
        stm->argumentos[i]->accept(this);
        valoresArgs.push_back(ultimoValorFloat);
    }
    
    // Guardar contexto anterior
    unordered_map<string, double> variablesAnteriores = variablesLocales;
    unordered_map<string, string> tiposAnteriores = tiposVariablesLocales;
    variablesLocales.clear();
    tiposVariablesLocales.clear();
    
    // Pasar parámetros
    for (size_t i = 0; i < valoresArgs.size(); i++) {
        if (i < funcion->Pnombres.size()) {
            variablesLocales[funcion->Pnombres[i]] = valoresArgs[i];
            // tiposVariablesLocales[funcion->Pnombres[i]] = funcion->Ptipos[i];
        }
    }
    
    scopeDepth++;
    hayRetorno = false;
    funcion->cuerpo->accept(this);
    scopeDepth--;
    
    // Restaurar contexto
    variablesLocales = variablesAnteriores;
    tiposVariablesLocales = tiposAnteriores;
    hayRetorno = false;
    
    // No usamos el valor de retorno, así que no hace falta tocar ultimoValorFloat
    return 0;
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
void EVALVisitor::interprete(Program* programa) {
    if (programa) {
        variablesGlobales.clear();
        variablesLocales.clear();
        tablaDeFunciones.clear();
        hayRetorno = false;
        valorRetorno = 0;
        scopeDepth = 0;  // Initialize scope depth
        
   
        programa->accept(this);
    }
}

