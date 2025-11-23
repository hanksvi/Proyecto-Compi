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


///////////////////////////////////////////////////////////////////////////////////


int GenCodeVisitor::generar(Program* program) {
    program->accept(this);
        return 0;
}

int GenCodeVisitor::visit(Program* program) {
out << ".data\nprint_fmt: .string \"%ld \\n\""<<endl;

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

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
        return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!entornoFuncion) {
            memoriaGlobal[var] = true;
        } else {
            memoria[var] = offset;
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
    if (memoriaGlobal.count(exp->value))
        out << " movq " << exp->value << "(%rip), %rax"<<endl;
    else
        out << " movq " << memoria[exp->value] << "(%rbp), %rax"<<endl;
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
        case LS_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
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
        out << " movq %rax, " << memoria[stm->id] << "(%rbp)"<<endl;
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



int GenCodeVisitor::visit(Body* b) {
    for (auto dec : b->declarations){
        dec->accept(this);
    }
    for (auto s : b->StmList){
        s->accept(this);
    }
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

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        memoria[f->Pnombres[i]]=offset;
        out << " movq " << argRegs[i] << "," << offset << "(%rbp)" << endl;
        offset -= 8;
    }
    for (auto i: f->cuerpo->declarations){
        i->accept(this);
    }
    int reserva = -offset - 8;
    out << " subq $" << reserva << ", %rsp" << endl;
    for (auto i: f->cuerpo->StmList){
        i->accept(this);
    }
    out << ".end_"<< f->nombre << ":"<< endl;
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

//////////////////////////////////////////////////////////
//PrintVisitor
//////////////////////////////////////////////////////////

int PrintVisitor::visit(NumberExp* exp){
    cout<< exp->value;
    return 0;
}

int PrintVisitor::visit(IdExp* exp){
    cout<< exp->value;
    return 0;
}

int PrintVisitor::visit(BinaryExp* exp){
    cout << "(";
    exp->left->accept(this);
    cout<<" "<<binopToChar(exp->op) << " ";
    exp->right->accept(this);
    cout<<")";
    return 0;
}

int PrintVisitor::visit(WhileStm* stm){
    cout<<"while (";
    stm->condition->accept(this);
    cout<<")";
    cout<<":\n";
    identLevel++;
    stm->b->accept(this); 
    identLevel--;
    return 0;
}

int PrintVisitor::visit(IfStm* stm){
    cout<<"if (";
    stm->condition->accept(this);
    cout<<")";
    cout<<":\n";
    identLevel++;
    stm->then->accept(this); 
    identLevel--;
    if (stm->els){
        cout<<"else:\n";
        identLevel++;
        stm->els->accept(this); 
        identLevel--;
    }
    return 0;
}

int PrintVisitor::visit(PrintStm* stm){
    cout<<"print (";
    stm->e->accept(this);
    cout<<")";
    return 0;
}

int PrintVisitor::visit(AssignStm* stm){
    cout<<stm->id<<" = ";
    stm->e->accept(this);
    return 0;
}

int PrintVisitor::visit(ReturnStm* stm){
    cout<<"return ";
    stm->e->accept(this);
    return 0;
}

int PrintVisitor::visit(VarDec* vd){
    cout<<"var";
    cout<<vd->type<<" ";
    for (auto i: vd->vars){
        cout<<","<< i <<" ";
    }
    cout<<";";
    return 0;
}


int PrintVisitor::visit(Body* b){
    for(auto i:b->declarations){
        i->accept(this);
    }
    for(auto i:b->StmList){
        i->accept(this);
    }
    return 0;
}

int PrintVisitor::visit(FcallExp* fcall){
    cout<<fcall->nombre<<"(";
    for(auto i:fcall->argumentos){
        i->accept(this);
    }
    cout<<")";
    return 0;
}


int PrintVisitor::visit(Fundec* fd){
    cout<<"proc"<<fd->nombre<<"(";
    for(size_t i=0; i< fd->Pnombres.size(); i++){
        cout<<fd->Pnombres[i]<<":"<<fd->Ptipos[i]<<",";
        if(i < fd->Pnombres.size()-1)
            cout<<",";
    }
    cout<<"): "<<fd->tipo<<" = "<<endl;
    identLevel++;
    fd->cuerpo->accept(this);
    identLevel--;
    return 0;
}


int PrintVisitor::visit(Program* p) {
    indent();
    vector<VarDec*> vars(p->vdlist.begin(), p->vdlist.end());
    vector<FunDec*> procs(p->fdlist.begin(), p->fdlist.end());
    vector<Stm*> stms(p->stmlist.begin(), p->stmlist.end());
    for(auto& item: p->order){
        if(item.first == "var"){
            vars[item.second]->accept(this);
        }
        else if(item.first == "proc"){
            procs[item.second]->accept(this);
        }
        else if(item.first == "stm"){
            stms[item.second]->accept(this);
        }
    }
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

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(IdExp* exp) {
    return env.lookup(exp->value);
}

int EVALVisitor::visit(BinaryExp* exp) {
    int left = exp->left->accept(this);
    int right = exp->right->accept(this);
    int result=0;

    switch(exp->op){
        case PLUS_OP:
            result = left + right;
            break;
        case MINUS_OP:
            result = left - right;
            break;
        case MUL_OP:
            result = left * right;
            break;
        case DIV_OP:
            if(right == 0){
                throw runtime_error("Division by zero");
            }
            result = left / right;
            break;
        case LS_OP:
            result = left < right;
            break;
        case LSEQ_OP:
            result = left <= right;
            break;
        case EQ_OP:
            result = left == right;
            break;
        case GR_OP:
            result = left > right;
            break;
        case GREQ_OP:
            result = left >= right;
            break;
        default:
            cout<<"OPERADOR DESCONOCIDO"<<endl;
            result = 0;
    }
    return result;
}


int EVALVisitor::visit(PrintStm* p) {
    cout<<p->e->accept(this);
    return 0;
}

int EVALVisitor::visit(AssignStm* p) {
    env.update(p->id,p->e->accept(this));
    return 0;
}

int EVALVisitor::visit(IfStm* p) {
    if (p->condition->accept(this)) {
        p->then->accept(this);
    }
    else if (p->els) {
        p->els->accept(this);
    }
    return 0;
}

int EVALVisitor::visit(WhileStm* p) {
    while (p->condition->accept(this)) {
        p->b->accept(this);
    }
    return 0;
}

int EVALVisitor::visit(ReturnStm* stm) {
    retcall= true;
    retval = stm->e->accept(this);
    return 0;
}

int EVALVisitor::visit(Vardec* vd){
    for(auto i:vd->vars){
        env.add_var(i,0);
    }
    return 0;
}

int EVALVisitor::visit(Body* b){
    env.add_level();
    for(auto i:b->declarations){
        i->accept(this);
    }

    for(auto i:b->StmList){
        i->accept(this);
    }
    env.remove_level();
    return 0;
}

int EVALVisitor::visit(FcallExp* fcall) {
    retcall = false;
    vector<int> arg;
    for (auto i : fcall->argumentos) {
        arg.push_back(i->accept(this));
    } 
    FunDec* fd = envfun[fcall->nombre];
    env.add_level();

    // Cargar los parámetros con sus valores
    for (size_t i = 0; i < arg.size(); ++i) {
        env.add_var(fd->Pnombres[i], arg[i]);
    }

    fd->cuerpo->accept(this);
    env.remove_level();

    if (retcall) {
        return retval;
    } else {
        cout << "Error: la función '" << fcall->nombre << "' no tiene retorno" << endl;
        exit(0);
    }
}

int EVALVisitor::visit(FunDec* fd) {
    envfun[fd->nombre] = fd;
    return 0;
}

int EVALVisitor::visit(Program* p) {
    env.add_level();    
    vector<VarDec*> vars(p->vdlist.begin(), p->vdlist.end());
    vector<FunDec*> procs(p->fdlist.begin(), p->fdlist.end());
    vector<Stm*> stms(p->stmlist.begin(), p->stmlist.end());
    for(auto& item: p->order){
        if(item.first == "var"){
            vars[item.second]->accept(this);
        }
        else if(item.first == "proc"){
            procs[item.second]->accept(this);
        }
        else if(item.first == "stm"){
            stms[item.second]->accept(this);
        }
    }
    
    env.remove_level();
    return 0;
}

void EVALVisitor::interprete(Program* programa) {
    if (programa) {
        cout<<"Interpretando..."<<endl;
        programa->accept(this);
    }
}
