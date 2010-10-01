#ifndef _types_h_included_
#define _types_h_included_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stack>
#include <vector>
#include <math.h>
#include <iostream>
#include <sstream>

#include <dlfcn.h>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <algorithm>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/ExecutionEngine/GenericValue.h> 

#include "helper_library.h"

using namespace llvm;
using namespace std;

class NStatement;
class NExpression;
class NVariableDeclaration;
class NExternalFunctionDeclaration;
class NBlock;
class CodeGenContext;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<string*> TypeList;
typedef std::vector<NVariableDeclaration*> VariableList;

static const Type* getTypeFromToken(string* token);

extern int getNextLine(void);
int GetNextChar(char* b, int maxBuffer);
extern void DumpRow(void); 
extern void BeginToken(char*);
extern void PrintError(char *s, ...);

/* extern int debug = 0; */

class Node {
 public:
  virtual ~Node() {}
  virtual Value *Codegen(CodeGenContext *context) =0;
};

class NExpression : public Node{

  //   virtual ~NExpression() {}
  //   virtual Value *Codegen(CodeGenContext *context) = 0;
  public:
  virtual string getNodeType(CodeGenContext* context) = 0;
};

class NStatement : public Node{

};

class FunctionContainer {
 public:
  std::vector<NBlock *> blocks;
  NBlock* currentBlock;
  FunctionContainer(NBlock* block=0) {if(block!=0) blocks.push_back(block); hasReturn=0;}
  Value* Codegen(CodeGenContext *context);
  string type;
  string name;
  VariableList arguments;
  void addArguments(VariableList Arguments);
  std::map<std::string, Value*> locals;
  std::map<std::string, std::string> localTypes;
  Function *function;
  FunctionType *ftype;
  int hasReturn;
};

class NBlock : public NExpression {
public:
  StatementList statements;
  NBlock() { block=0;function=0;}
  virtual Value* Codegen(CodeGenContext* context);
  virtual string getNodeType(CodeGenContext* context) { return "block";}
  BasicBlock* block;
  Function* function;
  string block_name;
  void createBasicBlock(CodeGenContext* context);
};

class NFunction {
 public:
  TypeList arguments;
  string return_type;
  string name;
  NExternalFunctionDeclaration* lib_func;
  int is_lib_func;
 NFunction(TypeList& Arguments, string* Return_type, string* Name) : arguments(Arguments), return_type(*Return_type), name(*Name) {is_lib_func=0;}
};

class NReturnStatement : public NStatement{
 public:
  NExpression* exp;
  NReturnStatement(NExpression* Exp) { exp = Exp; }
  virtual Value* Codegen(CodeGenContext* context);
};

class CodeGenContext {
 public:
  int block_number;
  Function *mainFunction;
  std::vector<FunctionContainer *> containers;
  map<string, NFunction*> functions;
  FunctionContainer* currentFunction;
  Module *module;
  CodeGenContext() { module = new Module("main", getGlobalContext());}
  void generateCode(std::vector<FunctionContainer *> &FunctionBlocks);
  void printLLVMIR();
  void runCode();
  void addLibFunction(NExternalFunctionDeclaration* func);
  std::map<std::string, Value*>& locals() {  return currentFunction->locals;}
  std::map<std::string, std::string>& localTypes() {  return currentFunction->localTypes;}
  BasicBlock* currentBlock() { return currentFunction->currentBlock->block;}
};

class NExpressionStatement : public NStatement {
public:
	NExpression* expression;
  NExpressionStatement(NExpression* Expression) {expression=Expression; }
  virtual Value* Codegen(CodeGenContext* context);
};

class NNumber : public NExpression {
 public:
  char type;
  string numType;
};

class NInteger : public NNumber{
 public:
  int value;
  NInteger(int val) {value = val; type='i'; numType="int";}
  virtual Value *Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) { return numType; }
};

class NFloat : public NNumber{
 public:
  double value;
  NFloat(double val) {value = val; type='f'; numType="float";}
  virtual Value *Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) { return numType; }
};

class NBinaryOp : public NExpression {
public:
  char Op;
  int elementwise_vector_op;
  NExpression *LHS, *RHS;
  NBinaryOp(char op, NExpression *lhs, NExpression *rhs) 
    : Op(op), LHS(lhs), RHS(rhs) {elementwise_vector_op=0;}
 NBinaryOp(char op, int elementwise, NExpression *lhs, NExpression *rhs) 
   : Op(op), elementwise_vector_op(elementwise), LHS(lhs), RHS(rhs) {}
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context);
  Value* arithmetic(CodeGenContext *context, Instruction::BinaryOps instr);
  Value* vector_arithmetic(CodeGenContext *context);
  Value* comparitive(CodeGenContext *context, char cmd);
  Value* power();
};

class NExternalFunctionDeclaration : public NStatement {
public:
  string name;
  string type;
  TypeList arguments;
  NBlock& block;
  NExternalFunctionDeclaration(const string* Name, const string *Type, const TypeList& Arguments, NBlock& Block) : arguments(Arguments), block(Block) , name(*Name), type(*Type){
// 	name = *Name;
// 	type = *Type;
// 	arguments = Arguments;
  }
  virtual Value* Codegen(CodeGenContext *context);
};

class NVariableDeclaration : public NStatement {

public:
  string name;
  string type;
  NExpression* rhs;
  FunctionContainer* function;
  NVariableDeclaration(string* NAME, NExpression* RHS=0, FunctionContainer* Function=0)
    { name = *NAME; rhs = RHS; if(Function!=0) {function=Function; }}

  NVariableDeclaration(string Type, string* NAME, NExpression* RHS=0, FunctionContainer* Function=0)
    { name = *NAME; rhs = RHS; type = Type; if(Function!=0) {function=Function; function->localTypes[name] = type;}}

  virtual Value* Codegen(CodeGenContext *context);
};


class NIndexedVectorVariableDeclaration : public NStatement {

public:
  string name;
  string type;
  NExpression* rhs;
  NExpression* index;
  FunctionContainer* function;
  NIndexedVectorVariableDeclaration(string* NAME,NExpression* Index, NExpression* RHS, FunctionContainer* Function) 
  { name = *NAME; rhs = RHS; index=Index;function=Function; }
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) { return type; }
};

class NIndexedVectorVariableReference : public NExpression {
public:
  string name;
  string type;
  NExpression* index;
  FunctionContainer* function;
  NIndexedVectorVariableReference(string* Name, NExpression* Index,FunctionContainer* Function);
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context)
  {
    if(function->localTypes[name] == "int[]") return "int";
    if(function->localTypes[name] == "float[]") return "float";
    else printf("something went wrong with getNodeType() for NIndeVVREf\n");
  }
};

class NAssignment : public NStatement {
public:
	string lhs;
	NExpression* rhs;
	NAssignment(string* lhs, NExpression* rhs) : 
		lhs(*lhs), rhs(rhs) { }
	virtual Value* Codegen(CodeGenContext *context);
};

NExpression* getNConsecutiveVectorDefinedByExp(NExpression* start, NExpression* step, NExpression* stop);


class NConsecutiveVector : public NExpression {
public:
  NExpression* start, *step, *end;
  string type;
  NConsecutiveVector(NExpression* Start, NExpression* Step, NExpression* End);
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) {return type;}
};

class NConsecutiveVectorFloat : public NExpression {
public:
  double start, step, end;
  NConsecutiveVectorFloat(double Start, double Step, double End) : start(Start), step(Step), end(End) { }
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) { return "float[]"; }
};

class NConsecutiveVectorInt : public NExpression {
public:
  int start, step, end;
  NConsecutiveVectorInt(int Start, int Step, int End) : start(Start), step(Step), end(End) { }
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) { return "int[]"; }
};
  
class NVariableReference : public NExpression {
public:
  string name;
  FunctionContainer* function;
  NVariableReference(string* NAME,FunctionContainer* Function) {name = *NAME; function = Function;}
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context);
};

class NVectorVariableReference : public NExpression {
public:
  string name;
  int index;
  NVectorVariableReference(string* Name, int Index) {name=*Name; index=Index;}
  virtual Value* Codegen(CodeGenContext *context);
  virtual string getNodeType(CodeGenContext* context) {return "int[]";}
};

class NMethodCall : public NExpression {
public:
  const string id;
  ExpressionList arguments;
  NMethodCall(const string& id, ExpressionList& arguments) :
    id(id), arguments(arguments) { }
  NMethodCall(const string& id) : id(id) { }
  virtual llvm::Value* Codegen(CodeGenContext* context);
  //TODO fix method return type!!!
  virtual string getNodeType(CodeGenContext* context) { return context->functions[id]->return_type; }
};


class NIfStatement : public NStatement {
 public:
  NExpression* condition;
  NBlock* body_block;
  NIfStatement(NExpression* cond, NBlock* block) {
	condition = cond; body_block=block;
  }
  virtual llvm::Value* Codegen(CodeGenContext* context);
};

class NForLoop : public NStatement {
public:
  string i;
  NBlock* body_block;
  
  string range_type;
  NConsecutiveVector* range;
  NConsecutiveVectorInt* intRange;
  NConsecutiveVectorFloat* floatRange;
  FunctionContainer* function;
  NForLoop(string iter_var, NConsecutiveVector* Range, NBlock*  Block,FunctionContainer* Function);
  virtual Value* Codegen(CodeGenContext* context);
};

#endif
