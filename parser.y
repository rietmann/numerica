%{

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include "types.h"
#define YYERROR_VERBOSE 1
  Module* makeLLVMModule();
  void yyerror (char const *s);
  NBlock *programBlock;
  std::vector<FunctionContainer *> FunctionBlocks;
  FunctionContainer* current_function;
  extern int yylex();
/* 	void yyerror(const char *s) { printf("ERROR: %s\n", s); } */
 %}

%union {
  int valInt;
  double valDouble;   /* For returning numbers.  */
  double valFloat;
  Node *node;
  NExpression *expr;
  NStatement *stmt;
  string *varName;
  string *typeName;
  string *langReserve;
  NBlock* block;
  NVariableDeclaration *var_decl;
  std::vector<NVariableDeclaration*> *varvec;
  std::vector<NExpression*> *exprvec; 
  TypeList *var_func_decl_args;
  FunctionContainer *function;
  VariableList *variable_list;
  NVariableDeclaration *varDec;
  NConsecutiveVector *rangeVec;
};
/* Bison declarations.  */

%token FLOAT INTEGER VARIABLE VARTYPE EXT_FUN_LOAD LEQ GEQ IF LBRKT RBRKT FOR
%token FUNCTION RETURN
%token                  END          0  "end of file"
%type<valFloat> FLOAT
%type<valInt> INTEGER
%type<varName> VARIABLE
%type<typeName> VARTYPE
%type<langReserve> EXT_FUN_LOAD IF LBRKT RBRKT FOR FUNCTION RETURN
/* %type<program_type> program */
%type<variable_list> func_decl_args;
%type<function> func_decl
%type<stmt> var_decl
%type<expr> exp
 /* %type<rangeVec> range */
%type<expr> range
%type<stmt> stmt

%type<stmt> if_statement for_loop
 /* %type <stmt> var_decl */
 /* func_decl */
 /* %type <varvec> func_decl_args */
%type <exprvec> call_args
%type<block> program block stmts

%type<var_func_decl_args> type_list;

%right '='
%left '<' '>' LEQ GEQ
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

%start program

%% /* The grammar follows.  */
 
program : stmts { FunctionBlocks.push_back(current_function); current_function->blocks.push_back($1); current_function = new FunctionContainer();}
| program func_decl { FunctionBlocks.push_back($2); }
;
  
func_decl : FUNCTION VARTYPE VARIABLE '(' func_decl_args ')' block
{ current_function->blocks.push_back($7); $$ = current_function; $$->type = *$2; $$->name=*$3; $$->addArguments(*$5); current_function = new FunctionContainer();}
;

func_decl_args: /*blank*/ {$$ = new VariableList(); }
| var_decl {  $$ = new VariableList(); $$->push_back($<var_decl>1);}
| func_decl_args ',' var_decl { $1->push_back($<var_decl>3); }
;

block : '{' stmts '}' { $$ = $2; }
;

if_statement: IF exp block {$$ = new NIfStatement($2,$3); printf("if@ %d\n",@1.first_line);}

for_loop: FOR VARIABLE '=' range block { $$ = new NForLoop(*$2, $<rangeVec>4, $5,current_function); } 

stmts : stmt {  $$ = new NBlock(); $$->statements.push_back($<stmt>1); } 
| stmts stmt { $$->statements.push_back($<stmt>2); }
;

stmt : var_decl  ';'  /*| func_decl*/
| exp ';'  { $$ = new NExpressionStatement($1); }
| EXT_FUN_LOAD VARTYPE VARIABLE '(' type_list ')' ';'  {$$ = new NExternalFunctionDeclaration($3, $2, *$5, *programBlock); }
/* | VARIABLE '=' exp ';'  {$$ = new NAssignment($1, $3);} */
| if_statement {$$ = $1;}
| for_loop { $$ = $1;}
| RETURN exp ';' { $$ = new NReturnStatement($2); }
;

type_list : /*blank*/ { $$ = new TypeList(); }
  | VARTYPE { $$ = new TypeList(); $$->push_back($1);}
  | type_list ',' VARTYPE {$1->push_back($3); }
;

// 'int' VARIABLE...
var_decl : VARTYPE VARIABLE '=' exp {$$ = new NVariableDeclaration(*$1,$2,$4,current_function);}
| VARTYPE VARIABLE '=' range {$$ = new NVariableDeclaration(*$1,$2,$4,current_function);}
| VARTYPE VARIABLE { $$ = new NVariableDeclaration(*$1,$2);}
| VARIABLE '=' exp { $$ = new NVariableDeclaration($1,$3,current_function);}
| VARIABLE '=' range {$$ = new NVariableDeclaration($1,$3,current_function);}
| VARIABLE'['exp']' '=' exp {$$ = new NIndexedVectorVariableDeclaration($1,$3,$6,current_function);}
;

range:  exp ':' exp ':' exp { $$ = new NConsecutiveVector($1, $3, $5); }
|  exp ':' exp { $$ = new NConsecutiveVector($1, new NInteger(1), $3); }
| '[' range ']' { $$ = $2; }
;

exp: VARIABLE { $$ = new NVariableReference($1, current_function); }
| VARIABLE '[' exp ']' { $$ = new NIndexedVectorVariableReference($1,$3,current_function);}
| FLOAT             { $$ = new NFloat($1);   }
| INTEGER             { $$ = new NInteger($1); }
| exp '+' exp        { $$ = new NBinaryOp('+',$1, $3); }
| exp '-' exp        { $$ = new NBinaryOp('-',$1, $3); }
| exp '*' exp        { $$ = new NBinaryOp('*',$1, $3); }
| exp '/' exp        { $$ = new NBinaryOp('/',$1, $3); }
| exp '<' exp        { $$ = new NBinaryOp('<',$1, $3); }
| exp '>' exp        { $$ = new NBinaryOp('>',$1, $3); }
| exp GEQ exp        { $$ = new NBinaryOp('g',$1, $3); }
| exp LEQ exp        { $$ = new NBinaryOp('l',$1, $3); }
| '-' exp  %prec NEG { $$ = new NBinaryOp('-',new NInteger(0),$2) }
| exp '^' exp        { $$ = new NBinaryOp('^',$1, $3); }
| '(' exp ')'        { $$ = $2;         }
| VARIABLE '(' call_args ')' { $$ = new NMethodCall(*$1, *$3); delete $3; }
;


call_args : /*blank*/  { $$ = new ExpressionList(); }
		  | exp { $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args ',' exp  { $1->push_back($3); }
		  ;

%%

/* void testRunning() { */
/*   CodeGenContext context; */
  
/*   Module *module = new Module("main", getGlobalContext()); */
/*   BasicBlock* BB = new BasicBlock */
/* } */




  
/* Called by yyparse on error.  */
void
yyerror (char const *s)
{
  fprintf (stderr, "Error:%s\n", s);
  exit(1);
}

