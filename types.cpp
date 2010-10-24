#include "types.h"
#include "parser.hpp"

#include <typeinfo>
#include <sstream>


Value* convertToDoubleIfNeeded(Value* exp, CodeGenContext* context);

Value* NFloat::Codegen(CodeGenContext *context) {
  return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value* NInteger::Codegen(CodeGenContext *context) {
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), value);
}

Value* NBinaryOp::comparitive(CodeGenContext* context,char cmp) {
  CmpInst::Predicate cmpinst;
  if(LHS->getNodeType(context) == "int" && RHS->getNodeType(context) == "int") {
    switch(cmp) {
    case '<': cmpinst = ICmpInst::ICMP_SLT; break;
    case '>': cmpinst = ICmpInst::ICMP_SGT; break;
    }
    return new ICmpInst(*(context->currentBlock()), cmpinst,LHS->Codegen(context) , RHS->Codegen(context), "");
  }
  else if((LHS->getNodeType(context) == "float" && RHS->getNodeType(context) == "int") ||
    (LHS->getNodeType(context) == "int" && RHS->getNodeType(context) == "float")
      || (LHS->getNodeType(context) == "float" && RHS->getNodeType(context) == "float")) {
    switch(cmp) {
    case '<' : cmpinst = FCmpInst::FCMP_OLT; break;
    case '>' : cmpinst = FCmpInst::FCMP_OGT; break;
    }
    return new FCmpInst(*(context->currentBlock()), cmpinst,
			convertToDoubleIfNeeded(LHS->Codegen(context),context),
			convertToDoubleIfNeeded(RHS->Codegen(context),context), "");
  }
    
}

Value* NBinaryOp::Codegen(CodeGenContext *context) {
  string rhs_type = RHS->getNodeType(context);
  string lhs_type = LHS->getNodeType(context);
  if( rhs_type.substr(rhs_type.length()-2,rhs_type.length()) == "[]" ||
	  lhs_type.substr(rhs_type.length()-1,lhs_type.length()) == "[]") {
	printf("vector types\n"); return vector_arithmetic(context);
  }
  switch(Op) {
  case '+': return this->arithmetic(context,Instruction::Add); break;
  case '-': return this->arithmetic(context,Instruction::Sub); break;
  case '*': return this->arithmetic(context,Instruction::FMul); break;
  case '/': return this->arithmetic(context,Instruction::FDiv); break;
  case '<': return this->comparitive(context,'<'); break;
  case '>': return this->comparitive(context,'>'); break;
  }
  return 0; 
}

Function* getVecLibFunction(string function_name, CodeGenContext* context) {
  Function* function = context->module->getFunction(function_name.c_str());
  if(!function) {
    NExternalFunctionDeclaration* func = context->functions[function_name]->lib_func;
    func->Codegen(context);
    function = context->module->getFunction(function_name.c_str());
  }
  if(function)
  return function;
  else {printf("no vector function found!?\n"); exit(1);}
}

Value* NBinaryOp::vector_arithmetic(CodeGenContext* context) {
  Function* function;
  std::vector<Value*> args;
  AllocaInst *alloc;
  string rhs_type = RHS->getNodeType(context);
  string lhs_type = LHS->getNodeType(context);

  string float_type = "float[]";
  string int_type = "int[]";
  // TODO: try to implement a more elegant way to do this later...
  if(Op == '+') {
    if(rhs_type == "float[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("addDoubleDoubleVectors",context);
      }
      else if(lhs_type == "int[]") {
	function = getVecLibFunction("addIntDoubleVectors",context);
      }
      alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
    }
    else if(rhs_type == "int[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("addDoubleIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
      }
      else if(lhs_type=="int[]") {
	function = getVecLibFunction("addIntIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&int_type), "temp", context->currentBlock());
      }
    }
  }
  else if(Op == '-') {
	if(rhs_type == "float[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("aMinusBDoubleDoubleVectors",context);
      }
      else if(lhs_type == "int[]") {
	function = getVecLibFunction("aMinusBIntDoubleVectors",context);
      }
      alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
    }
    else if(rhs_type == "int[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("aMinusBDoubleIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
      }
      else if(lhs_type=="int[]") {
	function = getVecLibFunction("aMinusBIntIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&int_type), "temp", context->currentBlock());
      }
    }
  }
  else if(Op == '*') {
    if(rhs_type == "float[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("mulDoubleDoubleVectors",context);
      }
      else if(lhs_type == "int[]") {
	function = getVecLibFunction("mulIntDoubleVectors",context);
      }
      alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
    }
    else if(rhs_type == "int[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("mulDoubleIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
      }
      else if(lhs_type=="int[]") {
	function = getVecLibFunction("mulIntIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&int_type), "temp", context->currentBlock());
      }
    }
  }
  else if(Op == '/') {
    if(rhs_type == "float[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("aDivBDoubleDoubleVectors",context);
      }
      else if(lhs_type == "int[]") {
	function = getVecLibFunction("aDivBIntDoubleVectors",context);
      }
      alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
    }
    else if(rhs_type == "int[]") {
      if(lhs_type == "float[]") {
	function = getVecLibFunction("aDivBDoubleIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
      }
      else if(lhs_type=="int[]") {
	function = getVecLibFunction("aDivBIntVectors",context);
	alloc = new AllocaInst(getTypeFromToken(&float_type), "temp", context->currentBlock());
      }
    }
  }
  Value* rhs=RHS->Codegen(context);
  Value* lhs=LHS->Codegen(context);
  
  args.push_back(alloc); // c = 
  args.push_back(lhs); // a [+-/*]
  args.push_back(rhs); // b
  
  CallInst *call = CallInst::Create(function, args.begin(), args.end(), "", context->currentBlock());
  printf("hello!\n");
  return new LoadInst(alloc, "",false, context->currentBlock());
}

string NBinaryOp::getNodeType(CodeGenContext* context) {
  if(!(LHS->getNodeType(context) == "int[]" || LHS->getNodeType(context) == "float[]")) {
    switch(Op) {
    case '/': return "float";
    case '^': return "float";
    }
  }
  if(LHS->getNodeType(context) == "int[]" && RHS->getNodeType(context) == "float")
    return "float[]";
  else if(LHS->getNodeType(context) == "float" && RHS->getNodeType(context) == "int[]")
    return "float[]";
  else if(LHS->getNodeType(context) == "float[]" || RHS->getNodeType(context) == "float[]")
    return "float[]";
  else if(LHS->getNodeType(context) == "float" || RHS->getNodeType(context) == "float")
    return "float";
  else if(LHS->getNodeType(context) == "int[]" || RHS->getNodeType(context) == "int[]")
    return "int[]";
  else if(LHS->getNodeType(context) == "int" && RHS->getNodeType(context) == "int")
    return "int";
  else
    printf("Should not be here!\n");
}

void FunctionContainer::addArguments(VariableList Arguments) {
  arguments = Arguments;
  VariableList::const_iterator it;
  if(arguments.size() > 0) {
    for (it = arguments.begin(); it != arguments.end(); it++) {
	  // store the type of the arguments for use within the function
      localTypes[(*it)->name] = (*it)->type;
    }
  }
}

Value* FunctionContainer::Codegen(CodeGenContext* context) {
  
  //   call codegen() on all blocks
  //   and tell current block that *this* is its function
  context->block_number = 0;
  currentBlock = blocks.front();
  currentBlock->function = function;
  currentBlock->block_name="entry";
  currentBlock->createBasicBlock(context);

  Function::arg_iterator args = function->arg_begin();
  VariableList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    Value* argi = args++;
    argi->setName((*it)->name);
    AllocaInst *alloc = new AllocaInst(getTypeFromToken(&((*it)->type)), (*it)->name.c_str(), context->currentBlock());
    Value* store_instr = new StoreInst(argi, alloc, false, context->currentBlock());
    locals[(*it)->name] = alloc;
    localTypes[(*it)->name] = (*it)->type;
  }
  
  currentBlock->Codegen(context);
  std::vector<NBlock*>::const_iterator itB;
  for (itB=blocks.begin()+1; itB < blocks.end(); itB++) {
        currentBlock = (*itB);
    std::stringstream ss;
    ss << "bb." << name << "." << context->block_number;
    currentBlock->block_name = ss.str();
    currentBlock->function = function;
    currentBlock->createBasicBlock(context);
    currentBlock->Codegen(context);
  }
  if(ftype->getReturnType()->isVoidTy()) {
    ReturnInst::Create(getGlobalContext(), context->currentBlock());
  }
  else if(!hasReturn) {
    ConstantInt* zero_return = ConstantInt::get(getGlobalContext(), APInt(32, StringRef("0"), 10));
    ReturnInst::Create(getGlobalContext(),zero_return, context->currentBlock()); 
  }
  return function;
}

Value* NExpressionStatement::Codegen(CodeGenContext *context) {
  return expression->Codegen(context);
}

void NBlock::createBasicBlock(CodeGenContext* context) {
  if(!block){
    block = BasicBlock::Create(getGlobalContext(), block_name.c_str(), function, 0);
    context->block_number++;
  }
}

Value* NBlock::Codegen(CodeGenContext *context) {
  StatementList::const_iterator it;

  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    //cannot get this "typeid" function to compile.. dunno what it does exactly
    //std::cout << "Generating code for " << typeid(**it).name() << endl;
    last = (**it).Codegen(context);

  }
  return last;
}

/* Returns an LLVM type based on the char type */
static const Type *typeOf(const char type) 
{
	if (type == 'i') {
		return Type::getInt64Ty(getGlobalContext());
	}
	else if (type == 'f') {
		return Type::getDoubleTy(getGlobalContext());
	}
	return Type::getVoidTy(getGlobalContext());
}

Value* NVariableDeclaration::Codegen(CodeGenContext *context) {
  type = rhs->getNodeType(context);
  if(!context->locals()[name] || context->localTypes()[name] != type) {
    AllocaInst *alloc = new AllocaInst(getTypeFromToken(&type), name.c_str(), context->currentBlock());
    context->locals()[name] = alloc;
    
    context->localTypes()[name] = type;
  }
  if (rhs != NULL) {
    printf("assigning %s as type:%s\n",name.c_str(),type.c_str());
    NAssignment assn(&name, rhs);
    assn.Codegen(context);
    printf("finished assignment for %s\n",name.c_str());
  }
  // return alloc;
}

Value* NIndexedVectorVariableDeclaration::Codegen(CodeGenContext *context) {
  //TODO: Error Checking
  type = rhs->getNodeType(context);
  if (context->locals().find(name) == context->locals().end()) {
    std::cerr << "undeclared variable " << name << endl;
    exit(1);
  }
  Value* rhs_value = rhs->Codegen(context);
   LoadInst* ptr_index = new LoadInst(context->locals()[name], "", false, context->currentBlock());
  Instruction* var_ptr = GetElementPtrInst::Create(ptr_index, index->Codegen(context), "", context->currentBlock());
  return new StoreInst(rhs_value, var_ptr, false, context->currentBlock());  
}
 
NIndexedVectorVariableReference::NIndexedVectorVariableReference(string* Name, NExpression* Index,FunctionContainer* Function) {
  name=*Name;
  function = Function;
  index=Index;
  
}

Value* NIndexedVectorVariableReference::Codegen(CodeGenContext *context) {
  
  if(index->getNodeType(context) == "int") {

  } // TODO: Add ability to index vector "int[]"
  else { printf("Variable Index must be an integer\n"); exit(1);}
  
  if (context->locals().find(name) == context->locals().end()) {
    std::cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  // ConstantInt* IndexConst = ConstantInt::get(getGlobalContext(), APInt(64, index, true));
  LoadInst* ptr_index = new LoadInst(context->locals()[name], "", false, context->currentBlock());
  //TODO: Do bounds checking here.
  Instruction* element_ptr = GetElementPtrInst::Create(ptr_index,index->Codegen(context) , "", context->currentBlock());
  return new LoadInst(element_ptr, "", false,
		      context->currentBlock());
}

Value* NAssignment::Codegen(CodeGenContext* context)
{
  if (context->locals().find((lhs)) == context->locals().end()) {
    std::cerr << "undeclared variable " << lhs << endl;
    return NULL;
  }
  
  Value* rhs_cg = rhs->Codegen(context);
  Value* instr = new StoreInst(rhs_cg, context->locals()[lhs], false, context->currentBlock());
  return instr;
}

Value* NVectorVariableReference::Codegen(CodeGenContext* context) {
  if (context->locals().find(name) == context->locals().end()) {
    std::cerr << "undeclared variable " << name << endl;
    return NULL;
  }
}

Value* NVariableReference::Codegen(CodeGenContext* context) {
  if (context->locals().find(name) == context->locals().end()) {
    std::cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  Value* load_ref = new LoadInst(context->locals()[name], "", false, context->currentBlock());
  return load_ref;
}

string NVariableReference::getNodeType(CodeGenContext* context) {
  if (function->localTypes.find(name) == function->localTypes.end()) {
    std::cerr << "undeclared variable in reference: " << name << endl;
    exit(1);
  }
  else return function->localTypes[name];
}

NConsecutiveVector::NConsecutiveVector(NExpression* Start, NExpression* Step, NExpression* End) {
  start=Start; step=Step; end=End;

  //TODO move all getNodeType references to Codegen()
  CodeGenContext* context;
  string start_type = start->getNodeType(context);
  string step_type = step->getNodeType(context);
  string end_type = end->getNodeType(context);
  
  if(start_type == "int" && step_type == "int" && end_type == "int") {
    type = "int[]";
  }
  else if(start_type != "int" && start_type != "float") {
    printf("start index must be of type int or float\n"); exit(1);
  }
  else if(step_type != "int" && step_type != "float") {
    printf("stepsize must be of type int or float\n"); exit(1);
  }
  else if(end_type != "int" && end_type != "float") {
    printf("stop index must be of type int or float\n"); exit(1);
  }
  else if(start_type == "float" || step_type == "float" || end_type == "float")
    type = "float[]"; // only choice left!
  else {
    printf("Error: Type weirdness!? Check NConsecutiveVector Constructor.\n");
    exit(1);
  }
}

Value* NConsecutiveVector::Codegen(CodeGenContext* context) {
  if(end>start) {
    Value* start_val = start->Codegen(context);
    Value* step_val = step->Codegen(context);
    Value* end_val = end->Codegen(context);
    Value* final_start, *final_step, *final_end;
    Function *makeVectorFunction;
    PointerType* PointerTy_0;
    if(type == "float[]") {
      // FP variables:
      makeVectorFunction = context->module->getFunction("makeEnumeratedVectorFloat");
      if(!makeVectorFunction) {
	vector<const Type*> argTypes;
	string float_type = "float";
	argTypes.push_back(getTypeFromToken(&float_type));
	argTypes.push_back(getTypeFromToken(&float_type));
	argTypes.push_back(getTypeFromToken(&float_type));
	FunctionType *ftype = FunctionType::get(getTypeFromToken(new string("float[]")),
						argTypes, false);
	makeVectorFunction = Function::Create(ftype, Function::ExternalLinkage,
					      "makeEnumeratedVectorFloat",
					      context->module);
	if(!makeVectorFunction) {
	  printf("Error Loading makeEnumeratedVectorFloat().\n");
	  exit(1);
	}
      }
      PointerTy_0 = PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);

      final_start = convertToDoubleIfNeeded(start_val,context);
      final_step = convertToDoubleIfNeeded(step_val,context);
      final_end = convertToDoubleIfNeeded(end_val,context);
      
    }
    else if(type=="int[]") {
      makeVectorFunction = context->module->getFunction("makeEnumeratedVectorInteger");
      PointerTy_0 = PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
      final_start = start_val;
      final_step = step_val;
      final_end = end_val;
      if(!makeVectorFunction) {
	printf("Loading vector_int function for first time\n");
	vector<const Type*> argTypes;
	string int_type = "int";
	argTypes.push_back(getTypeFromToken(&int_type));
	argTypes.push_back(getTypeFromToken(&int_type));
	argTypes.push_back(getTypeFromToken(&int_type));
	FunctionType *ftype = FunctionType::get(getTypeFromToken(new string("int[]")),
						argTypes, false);
	makeVectorFunction = Function::Create(ftype, Function::ExternalLinkage,
					      "makeEnumeratedVectorInteger",
					      context->module);
	if(!makeVectorFunction) {
	  printf("Error Loading makeEnumeratedVectorInteger().\n");
	  exit(1);
	}
      }
    }
    else {
      printf("Vector didn't have a correct type (Codegen)\n"); exit(1);
    }

    // regular variables:

    std::vector<Value*> ptr_vector_return_params;
    
    ptr_vector_return_params.push_back(final_start);
    ptr_vector_return_params.push_back(final_step);
    ptr_vector_return_params.push_back(final_end);

    CallInst* ptr_vector_return = CallInst::Create(makeVectorFunction, ptr_vector_return_params.begin(), ptr_vector_return_params.end(), "", context->currentBlock());

    ptr_vector_return->setCallingConv(CallingConv::C);
    ptr_vector_return->setTailCall(false);AttrListPtr ptr_vector_return_PAL;
    ptr_vector_return->setAttributes(ptr_vector_return_PAL);
    return ptr_vector_return;
    // new StoreInst(ptr_vector_return, ptr_vector, false, context->currentBlock());
  }
  else { printf("range end was before beginning!\n"); exit(1);}
  
}

Value* NConsecutiveVectorFloat::Codegen(CodeGenContext* context) {
  
  if(end>start) {

    Function *makeVectorFunction = context->module->getFunction("makeEnumeratedVectorFloat");
    if(!makeVectorFunction) {printf("Error Loading vector function\n"); exit(1);}
    PointerType* PointerTy_0 = PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);
    AllocaInst* ptr_vector = new AllocaInst(PointerTy_0, "", context->currentBlock());
    std::vector<Value*> ptr_vector_return_params;
    ConstantFP* CFP_start = ConstantFP::get(getGlobalContext(), APFloat(start));
    ConstantFP* CFP_step = ConstantFP::get(getGlobalContext(), APFloat(step));
    ConstantFP* CFP_end = ConstantFP::get(getGlobalContext(), APFloat(end));
    ptr_vector_return_params.push_back(CFP_start);
    ptr_vector_return_params.push_back(CFP_step);
    ptr_vector_return_params.push_back(CFP_end);

    CallInst* ptr_vector_return = CallInst::Create(makeVectorFunction, ptr_vector_return_params.begin(), ptr_vector_return_params.end(), "", context->currentBlock());

    ptr_vector_return->setCallingConv(CallingConv::C);
    ptr_vector_return->setTailCall(false);AttrListPtr ptr_vector_return_PAL;
    ptr_vector_return->setAttributes(ptr_vector_return_PAL);
    return ptr_vector_return;
    // new StoreInst(ptr_vector_return, ptr_vector, false, context->currentBlock());
  }
  else { printf("range end was before beginning!\n"); exit(1);}
}

Value* NConsecutiveVectorInt::Codegen(CodeGenContext* context) {
  if(end>start) {

    Function *makeVectorFunction = context->module->getFunction("makeEnumeratedVectorInteger");
    if(!makeVectorFunction) {printf("Error Loading vector function\n"); exit(1);}
    PointerType* PointerTy_0 = PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
    AllocaInst* ptr_vector = new AllocaInst(PointerTy_0, "", context->currentBlock());
    std::vector<Value*> ptr_vector_return_params;
    ConstantInt* CI_start = ConstantInt::get(getGlobalContext(), APInt(32,start,true));
    ConstantInt* CI_step = ConstantInt::get(getGlobalContext(), APInt(32,step,true));
    ConstantInt* CI_end = ConstantInt::get(getGlobalContext(), APInt(32,end,true));
    ptr_vector_return_params.push_back(CI_start);
    ptr_vector_return_params.push_back(CI_step);
    ptr_vector_return_params.push_back(CI_end);

    CallInst* ptr_vector_return = CallInst::Create(makeVectorFunction, ptr_vector_return_params.begin(), ptr_vector_return_params.end(), "", context->currentBlock());

    ptr_vector_return->setCallingConv(CallingConv::C);
    ptr_vector_return->setTailCall(false);AttrListPtr ptr_vector_return_PAL;
    ptr_vector_return->setAttributes(ptr_vector_return_PAL);
    return ptr_vector_return;
    // new StoreInst(ptr_vector_return, ptr_vector, false, context->currentBlock());
  }
  else { printf("range end was before beginning!\n"); exit(1);}
}

// checks if the value is an integer, in which case it is cast to a
// double. If the expression is already a double, it will be left
// alone.
Value* convertToDoubleIfNeeded(Value* exp, CodeGenContext* context) {
  if(exp->getType()->isIntegerTy()) {
    return new SIToFPInst(exp, Type::getDoubleTy(getGlobalContext()),"",
			  context->currentBlock());
  }
  else
    return exp;
}

Value* convertToIntegerIfNeeded(Value* exp, CodeGenContext* context) {
  if(exp->getType()->isDoubleTy()) {
    return new FPToSIInst(exp, Type::getInt32Ty(getGlobalContext()),"",
			  context->currentBlock());
  }
  else
    return exp;
}

Value* NBinaryOp::arithmetic(CodeGenContext *context, Instruction::BinaryOps instr) {
  Value* lhs = LHS->Codegen(context);
  Value* rhs = RHS->Codegen(context);
  Value* final_lhs;
  Value* final_rhs;

  // 1. if either operand is a double, we check both and casts the
  // offending integer where necessary.
  // *******
  // 2. If the operands are to be DIVIDED, we cast them as floats and use
  // floating point division.
  if(   lhs->getType()->isDoubleTy()
     || rhs->getType()->isDoubleTy()
     || instr==Instruction::FDiv) {
    final_lhs = convertToDoubleIfNeeded(lhs,context);
    final_rhs = convertToDoubleIfNeeded(rhs,context);
    if(instr==Instruction::Mul) {
      instr = Instruction::FMul;
    }

  }
  else { // Both operands are integers, so there is no need for casting.
    final_lhs = lhs;
    final_rhs = rhs;
  }
    
  Value* Operator = BinaryOperator::Create(instr, final_lhs, final_rhs, "",
					   context->currentBlock());
  return Operator;
}

static const Type* getTypeFromToken(string* token) {
  if(*token == "int") {
	return Type::getInt32Ty(getGlobalContext());
  }
  else if (*token == "float") {
	printf("here!\n");
	return Type::getDoubleTy(getGlobalContext());
  }
  else if (*token == "float[]") {
    return PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);
  }
  else if (*token == "int[]") {
    return PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
  }
  else if (*token == "float*[]") {
    return PointerType::get(PointerType::get(Type::getDoubleTy(getGlobalContext()), 0),0);
  }
  else if (*token == "int*[]") {
    PointerType* PointerTy_2 = PointerType::get(IntegerType::get(getGlobalContext(), 32), 0);
    return PointerType::get(PointerTy_2, 0);
    // return PointerType::get(PointerType::get(Type::getInt32Ty(getGlobalContext()), 0),0);
  }
  else
	return Type::getVoidTy(getGlobalContext());
}

Value* NReturnStatement::Codegen(CodeGenContext* context) {
  printf("generating return inst\n");
  if(context->currentFunction->type == "int") {
    Value* expr = exp->Codegen(context);
    expr = convertToIntegerIfNeeded(expr,context);
    ReturnInst::Create(getGlobalContext(),expr, context->currentBlock());
  }
  else if(context->currentFunction->type == "float") {
    Value* expr = exp->Codegen(context);
    expr = convertToDoubleIfNeeded(expr,context);
    ReturnInst::Create(getGlobalContext(),expr, context->currentBlock());
  }
  context->currentFunction->hasReturn = 1;

}

Value* NExternalFunctionDeclaration::Codegen(CodeGenContext* context) {
  context->functions[name] = new NFunction(arguments, &type, &name);
  vector<const Type*> argTypes;
  TypeList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
	argTypes.push_back(getTypeFromToken((*it)));
  }
  FunctionType *ftype = FunctionType::get(getTypeFromToken(&type), argTypes, false);
  Function *function = Function::Create(ftype, Function::ExternalLinkage, name.c_str(), context->module);

  return function;
}

void CodeGenContext::printLLVMIR() {
  if(module) {
    printf("---------LLVM IR---------\n");
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
  }
  else printf("IR not yet generated");
}

void CodeGenContext::addLibFunction(NExternalFunctionDeclaration* func) {
  functions[func->name] = new NFunction(func->arguments,&(func->type),&(func->name));
  functions[func->name]->is_lib_func = 1;
  functions[func->name]->lib_func = func;
}

void CodeGenContext::generateCode(std::vector<FunctionContainer *> &FunctionBlocks)
{
	containers = FunctionBlocks;
	std::vector<FunctionContainer*>::const_iterator it;
	for (it = containers.begin(); it != containers.end(); it++) {
	  FunctionContainer *current = *it;
	  
	  vector<const Type*> argTypes;
	  TypeList argTypes_string;
	  VariableList::const_iterator it2;
	  if(current->arguments.size() > 0) {
	    for (it2 = current->arguments.begin(); it2 != current->arguments.end(); it2++) {
	      argTypes.push_back(getTypeFromToken(&((*it2)->type)));
	      argTypes_string.push_back(&((*it2)->type));
	    }
	  }
	  
	  this->functions[current->name] = new NFunction(argTypes_string,&(current->type), &(current->name));
	  
	  if(current->type.length() > 0) {
		printf("%s has return type: %s\n",current->name.c_str(), current->type.c_str());
	    current->ftype = FunctionType::get(getTypeFromToken(&(current->type)), argTypes, false);
	  }
	  else {
	    current->ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
	  }
  
	  //if it doesn't have a name, it has to be main function	  
	  if(current->name.length() == 0) { current->name = "main"; }
	  printf("generating: %s\n",current->name.c_str());
	  Function* func = Function::Create(current->ftype,
				      GlobalValue::InternalLinkage,
				      current->name.c_str(), this->module);
	  printf("generated!\n");
	  current->function = func;
	  if(current->name=="main") this->mainFunction = current->function;
	  
	}

	/* generate code in all functions, now that they are all registered and generated
	  */
	
	for (it = containers.begin(); it != containers.end(); it++) {
	  currentFunction = (*it);
	  (**it).Codegen(this);
	}


}

void CodeGenContext::runCode() {

  verifyFunction(*mainFunction);
  ExecutionEngine* EE = EngineBuilder(module).create();

  std::vector<GenericValue> noargs;
  printf("--------Execution---------\n");
  GenericValue gv = EE->runFunction(mainFunction, noargs);
  
}

Value* NMethodCall::Codegen(CodeGenContext* context)
{
  Function *function = context->module->getFunction(id.c_str());
  
  ExpressionList::const_iterator method_call_argument_iterator;

  if (function == NULL) {
    // auto-load function when needed (try to at least)
    NExternalFunctionDeclaration* func = context->functions[id]->lib_func;
    
    if(!func) {
      std::cerr << "no such function " << id << endl; exit(1);
    }
    
    func->Codegen(context);
    function = context->module->getFunction(id.c_str());
  }
  
  std::vector<Value*> args;
  
  if(arguments.size() != context->functions[id]->arguments.size()) {
    printf("Error: Method call and definition have different number of arguments\n");exit(1);}
  
  TypeList::const_iterator method_def_argument_it;
  method_def_argument_it=context->functions[id]->arguments.begin();
  
  for (method_call_argument_iterator = arguments.begin(); method_call_argument_iterator != arguments.end(); method_call_argument_iterator++) {

    Value* arg_val;
	if( (**method_call_argument_iterator).getNodeType(context) != (**method_def_argument_it) ) {

      if((**method_call_argument_iterator).getNodeType(context) == "int" && (**method_def_argument_it) == "float") {
		arg_val = convertToDoubleIfNeeded((**method_call_argument_iterator).Codegen(context),context);
      }
      else if((**method_call_argument_iterator).getNodeType(context) == "float" && (**method_def_argument_it) == "int"){
	printf("Error: Float not converted to integer\n"); exit(1);}
      else {
	printf("Error: Type conversion to %s not performed\n",(**method_def_argument_it).c_str()); exit(1);
      }
    }
    else {arg_val=(**method_call_argument_iterator).Codegen(context);}
    args.push_back(arg_val);
	method_def_argument_it++;
  }

  CallInst *call = CallInst::Create(function, args.begin(), args.end(), "", context->currentBlock());
  return call;
}

Value* NIfStatement::Codegen(CodeGenContext* context) {
  
  Value* conditional = condition->Codegen(context);
  body_block->function = context->currentFunction->function;
  std::stringstream ss;

  ss << "bbranch." << context->currentFunction->name << "." << context->block_number;
  body_block->block_name = ss.str();
  body_block->createBasicBlock(context);
  NBlock *follow_block = new NBlock();

  follow_block->function = context->currentFunction->function;
  std::stringstream ss2;
  ss2 << "bb." << context->currentFunction->name << "." << context->block_number;
  follow_block->block_name = ss2.str();
  follow_block->createBasicBlock(context);
  
  Value * branch = BranchInst::Create(body_block->block, follow_block->block, conditional, context->currentBlock());
  
  std::vector<NStatement*> *stmts = &(context->currentFunction->currentBlock->statements);
  for(int i=0;stmts->size(); i++) { 
    if((*stmts)[i] == this) {
      i++;
      std::vector<NStatement*>::iterator it = stmts->begin();
      for(int j=i;j<stmts->size();) {
	follow_block->statements.push_back((*stmts)[j]);
	it+=j;
	stmts->erase(it);
	it=stmts->begin();
      }
      break;	
    }
  }
  NBlock *prev;
  prev = context->currentFunction->currentBlock;
  context->currentFunction->currentBlock=body_block;
  body_block->Codegen(context);
  context->currentFunction->blocks.push_back(follow_block);
  BranchInst::Create(follow_block->block, body_block->block);
  context->currentFunction->currentBlock = prev;
  return branch;
}

NForLoop::NForLoop(string iter_var, NConsecutiveVector* Range,
		   NBlock*  Block,FunctionContainer* Function)
{range=Range; i = iter_var; body_block=Block;
  CodeGenContext* context;
  range_type = range->getNodeType(context); function=Function;}

Value* NForLoop::Codegen(CodeGenContext* context) {

  // move the rest of the instructions to the follow block from the
  // current block so that they will be executed after the loop
  
  NBlock *follow_block = new NBlock();
  follow_block->function = context->currentFunction->function;
  
  std::vector<NStatement*> *stmts = &(context->currentFunction->currentBlock->statements);
  for(int k=0;stmts->size(); k++) { 
    if((*stmts)[k] == this) {
      k++;
      std::vector<NStatement*>::iterator it = stmts->begin();
      for(int j=k;j<stmts->size();) {
	follow_block->statements.push_back((*stmts)[j]);
	it+=j;
	stmts->erase(it);
	it=stmts->begin();
      }
      break;	
    }
  }

  // prepate iterator i
  string iter_type = range_type.substr(0,range_type.length()-2);
  NVariableDeclaration* iter = new NVariableDeclaration(iter_type, &i,range->start, function);
  Value* i_val = iter->Codegen(context);

  // prepare the two blocks of the loop("bfor_check", "bfor_body") and
  // the block to follow (follow_block="bb.fcn.#")
  std::stringstream ss;
  ss << "bfor_check." << context->currentFunction->name << "." << context->block_number;
  NBlock* check_block = new NBlock();
  check_block->function = context->currentFunction->function;
  check_block->block_name = ss.str();
  check_block->createBasicBlock(context);

  //go into cmp block of for loop
  Value* branch_into_loop = BranchInst::Create(check_block->block, context->currentBlock());
  
  NBlock *prev;
  prev = context->currentFunction->currentBlock;

  context->currentFunction->currentBlock = check_block;
  ss.str("");
  ss << "bfor_body." << context->currentFunction->name << "." << context->block_number;
  body_block->function = context->currentFunction->function;
  body_block->block_name = ss.str();
  body_block->createBasicBlock(context);
  
  ss.str("");
  ss << "bb." << context->currentFunction->name << "." << context->block_number;
  follow_block->block_name = ss.str();
  follow_block->createBasicBlock(context);


  //now we check i to see if we keep looping
  NVariableReference* iref = new NVariableReference(&i,context->currentFunction);

  Value* i_val_cmp = iref->Codegen(context);
  Value* range_end_val = range->end->Codegen(context);
  Value* cmp;
  if(iter_type=="int") {

    cmp = new ICmpInst(*(context->currentBlock()),
		       ICmpInst::ICMP_SLE, i_val_cmp, range_end_val, "");
  }
  else if(iter_type=="float") {
    cmp = new FCmpInst(*(context->currentBlock()),
		       FCmpInst::FCMP_OLE,
		       i_val_cmp,
		       convertToDoubleIfNeeded(range_end_val,context), "");  
  }
  Value* branch = BranchInst::Create(body_block->block, follow_block->block, cmp, context->currentBlock());

  // body of for loop
  context->currentFunction->currentBlock=body_block;
  body_block->Codegen(context);
  //increment i by step

  
  NBinaryOp* add = new NBinaryOp('+',iref, range->step);
  NAssignment* i_add_and_store = new NAssignment(&i,add);
  i_add_and_store->Codegen(context);
  // branch back to cmp block
  BranchInst::Create(check_block->block, body_block->block);
  context->currentFunction->blocks.push_back(follow_block);
  context->currentFunction->currentBlock = prev;
  return branch_into_loop;  
}



// /*================================================================*/
//   /* opening input -------------------------------------------------*/
//   printf("reading file '%s'\n", infile);
//   file = fopen(infile, "r");
//   if (  file == NULL  ) {
//     printf("cannot open input\n");
//     return 12;
//     }

//   /*================================================================*/
//   /*----------------------------------------------------------------*/
//   buffer = malloc(lMaxBuffer);
//   if (  buffer == NULL  ) {
//     printf("cannot allocate %d bytes of memory\n", lMaxBuffer);
//     fclose(file);
//     return 12;
//   }

//   /*================================================================*/
//   /* parse it ------------------------------------------------------*/
//   DumpRow();
//   if (  getNextLine() == 0  )
//     yyparse();
