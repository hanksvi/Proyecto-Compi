# Proyecto-Compi

# Gramatica:
Program ::= VarDecList  FunDecList  Body
VarDecList ::= (VarDec)*
FunDecList ::= (FunDec)*
FunDec ::= 'proc' id '(' [ParamDecList] ')' [':' Type] '=' NEWLINE INDENT Body DEDENT 
Body ::= VarDecList StmtList
ParamDecList ::= Param (',' Param)*
Param ::= id ':' Type	
VarDec ::= 'v	ar' VarList ':' Type
VarList ::= id (',' id)*
StmtList ::= Stmt (NEWLINE Stmt )*
Stmt ::= id '=' CExp
| 'echo' '(' CExp ')'
| 'if' CExp ':' NEWLINE INDENT Body ['else' ':' NEWLINE INDENT Body DEDENT] DEDENT
| 'while' CExp ':' NEWLINE INDENT Body DEDENT
| 'return' [CExp]
CExp ::= Exp (('<' | '<=' | '==' | '>=' | '>') Exp)?
Exp ::= Term (('+' | '-') Term)*
Term ::= Factor (('*' | '/') Factor)*
Factor ::= id | Num | Bool | '(' Exp ')' | id '(' [ArgList] ')' 
ArgList ::= CExp (',' CExp)*
Bool ::= 'true' | 'false'
Type ::= id
