#include "types.h"
#include "parser.hpp"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <ctype.h>

FILE *file;
int eof = 0;
int nRow = 0;
int nBuffer = 0;
int lBuffer = 0;
int nTokenStart = 0;
int nTokenLength = 0;
int nTokenNextStart = 0;
int lMaxBuffer = 1000;
char *buffer;

extern NBlock *programBlock;
extern std::vector<FunctionContainer *> FunctionBlocks;
extern FunctionContainer* current_function;

extern int yylex(void);
extern int yyparse(void);
extern void yyerror(char*);

void test(void) {
  printf("test()\n");
}

int debug = 0;

/*               Fancy Dancy Input Handler                        */  
/*================================================================*/
/*----------------------------------------------------------------*/
static
char dumpChar(char c) {
  if (  isprint(c)  )
    return c;
  return '@';
}

static
char *dumpString(char *s) {
  static char buf[101];
  int i;
  int n = strlen(s);

  if (  n > 100  )
    n = 100;

  for (i=0; i<n; i++)
    buf[i] = dumpChar(s[i]);
  buf[i] = 0;
  return buf;
}

extern
void DumpRow(void) {
  if( nRow != 0)  
    fprintf(stdout, "%6d |%.*s", nRow, lBuffer, buffer);
}

extern
int getNextLine(void) {
  int i;
  char *p;
  
  /*================================================================*/
  /*----------------------------------------------------------------*/
  nBuffer = 0;
  nTokenStart = -1;
  nTokenNextStart = 1;
  eof = false;

  /*================================================================*/
  /* read a line ---------------------------------------------------*/
  p = fgets(buffer, lMaxBuffer, file);
  if (  p == NULL  ) {
    if (  ferror(file)  )
      return -1;
    eof = true;
    return 1;
  }

  nRow += 1;
  lBuffer = strlen(buffer);
  DumpRow();

  /*================================================================*/
  /* that's it -----------------------------------------------------*/
  return 0;
}

/*--------------------------------------------------------------------
 * GetNextChar
 * 
 * reads a character from input for flex
 *------------------------------------------------------------------*/
extern
int GetNextChar(char* b, int maxBuffer) {
  int frc;
  
  /*================================================================*/
  /*----------------------------------------------------------------*/
  if (  eof  )
    return 0;
  
  /*================================================================*/
  /* read next line if at the end of the current -------------------*/
  while (  nBuffer >= lBuffer  ) {
    frc = getNextLine();
    if (  frc != 0  )
      return 0;
    }

  /*================================================================*/
  /* ok, return character ------------------------------------------*/
  b[0] = buffer[nBuffer];
  nBuffer += 1;

  if (  debug  )
    printf("GetNextChar() => '%c'0x%02x at %d\n",
                        dumpChar(b[0]), b[0], nBuffer);
  return b[0]==0?0:1;
}

extern
void BeginToken(char *t) {
  /*================================================================*/
  /* remember last read token --------------------------------------*/
  nTokenStart = nTokenNextStart;
  nTokenLength = strlen(t);
  nTokenNextStart = nBuffer; // + 1;

  /*================================================================*/
  /* location for bison --------------------------------------------*/
  yylloc.first_line = nRow;
  yylloc.first_column = nTokenStart;
  yylloc.last_line = nRow;
  yylloc.last_column = nTokenStart + nTokenLength - 1;

  if (  debug  ) {
    printf("Token '%s' at %d:%d next at %d\n", dumpString(t),
                        yylloc.first_column,
                        yylloc.last_column, nTokenNextStart);
  }
}


void addLibraryFunctions(CodeGenContext* context) {
  TypeList* print_arg_types = new TypeList();
  print_arg_types->push_back(new string("float"));
  
  TypeList* printi_arg_types = new TypeList();
  printi_arg_types->push_back(new string("int"));

  NExternalFunctionDeclaration* printfunc = new NExternalFunctionDeclaration(new string("print"), new string("void"), *print_arg_types, *programBlock);

  NExternalFunctionDeclaration* printifunc = new NExternalFunctionDeclaration(new string("printi"), new string("void"), *printi_arg_types, *programBlock);
  
  context->addLibFunction(printfunc);
  context->addLibFunction(printifunc);

  TypeList vecIntTypeList;
  vecIntTypeList.push_back(new string("int[]"));
  TypeList vecDoubleTypeList;
  vecIntTypeList.push_back(new string("float[]"));
  NExternalFunctionDeclaration* lengthi_func =
    new NExternalFunctionDeclaration(new string("lengthi"), new string("int"),
				     vecIntTypeList, *programBlock);
  NExternalFunctionDeclaration* lengthf_func =
    new NExternalFunctionDeclaration(new string("lengthf"), new string("int"),
				     vecDoubleTypeList, *programBlock);
  
  
  TypeList* doubleDoubleTypes = new TypeList();
  doubleDoubleTypes->push_back(new string("float*[]"));
  doubleDoubleTypes->push_back(new string("float[]"));
  doubleDoubleTypes->push_back(new string("float[]"));
  TypeList* intDoubleTypes = new TypeList();
  intDoubleTypes->push_back(new string("float*[]"));
  intDoubleTypes->push_back(new string("int[]"));
  intDoubleTypes->push_back(new string("float[]"));
  TypeList* doubleIntTypes = new TypeList();
  doubleIntTypes->push_back(new string("float*[]"));
  doubleIntTypes->push_back(new string("float[]"));
  doubleIntTypes->push_back(new string("int[]"));
  TypeList* intIntTypes = new TypeList();
  intIntTypes->push_back(new string("int*[]"));
  intIntTypes->push_back(new string("int[]"));
  intIntTypes->push_back(new string("int[]"));

  TypeList* justIntType = new TypeList();
  justIntType->push_back(new string("int*[]"));
  NExternalFunctionDeclaration* makeEmptyVector_func =
    new NExternalFunctionDeclaration(new string("makeEmptyVector"),
						     new string("void"),
						     *justIntType,
						     *programBlock);
  
  NExternalFunctionDeclaration* addDoubleDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("addDoubleDoubleVectors"),
				     new string("void"), *doubleDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* addIntDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("addIntDoubleVectors"),
				     new string("void"), *intDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* addDoubleIntVectors_func =
    new NExternalFunctionDeclaration(new string("addDoubleIntVectors"),
				     new string("void"), *doubleIntTypes,
				     *programBlock);
  NExternalFunctionDeclaration* addIntIntVectors_func =
    new NExternalFunctionDeclaration(new string("addIntIntVectors"),
				     new string("void"), *intIntTypes,
				     *programBlock);
  context->addLibFunction(makeEmptyVector_func);
  context->addLibFunction(addDoubleDoubleVectors_func);
  context->addLibFunction(addIntDoubleVectors_func);
  context->addLibFunction(addDoubleIntVectors_func);
  context->addLibFunction(addIntIntVectors_func);

  NExternalFunctionDeclaration* aMinusBDoubleDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("aMinusBDoubleDoubleVectors"),
				     new string("void"), *doubleDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* aMinusBIntDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("aMinusBIntDoubleVectors"),
				     new string("void"), *intDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* aMinusBDoubleIntVectors_func =
    new NExternalFunctionDeclaration(new string("aMinusBDoubleIntVectors"),
				     new string("void"), *doubleIntTypes,
				     *programBlock);
  NExternalFunctionDeclaration* aMinusBIntIntVectors_func =
    new NExternalFunctionDeclaration(new string("aMinusBIntIntVectors"),
				     new string("void"), *intIntTypes,
				     *programBlock);
  context->addLibFunction(aMinusBDoubleDoubleVectors_func);
  context->addLibFunction(aMinusBIntDoubleVectors_func);
  context->addLibFunction(aMinusBDoubleIntVectors_func);
  context->addLibFunction(aMinusBIntIntVectors_func);
  
  NExternalFunctionDeclaration* mulDoubleDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("mulDoubleDoubleVectors"),
				     new string("void"), *doubleDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* mulIntDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("mulIntDoubleVectors"),
				     new string("void"), *intDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* mulDoubleIntVectors_func =
    new NExternalFunctionDeclaration(new string("mulDoubleIntVectors"),
				     new string("void"), *doubleIntTypes,
				     *programBlock);
  NExternalFunctionDeclaration* mulIntIntVectors_func =
    new NExternalFunctionDeclaration(new string("mulIntIntVectors"),
				     new string("void"), *intIntTypes,
				     *programBlock);
  context->addLibFunction(mulDoubleDoubleVectors_func);
  context->addLibFunction(mulIntDoubleVectors_func);
  context->addLibFunction(mulDoubleIntVectors_func);
  context->addLibFunction(mulIntIntVectors_func);

  NExternalFunctionDeclaration* aDivBDoubleDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("aDivBDoubleDoubleVectors"),
				     new string("void"), *doubleDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* aDivBIntDoubleVectors_func =
    new NExternalFunctionDeclaration(new string("aDivBIntDoubleVectors"),
				     new string("void"), *intDoubleTypes,
				     *programBlock);
  NExternalFunctionDeclaration* aDivBDoubleIntVectors_func =
    new NExternalFunctionDeclaration(new string("aDivBDoubleIntVectors"),
				     new string("void"), *doubleIntTypes,
				     *programBlock);
  TypeList* returnFloat_intIntTypes = new TypeList();
  intIntTypes->push_back(new string("float*[]"));
  intIntTypes->push_back(new string("int[]"));
  intIntTypes->push_back(new string("int[]"));
  NExternalFunctionDeclaration* aDivBIntIntVectors_func =
    new NExternalFunctionDeclaration(new string("aDivBIntIntVectors"),
				     new string("void"), *returnFloat_intIntTypes,
				     *programBlock);
  context->addLibFunction(aDivBDoubleDoubleVectors_func);
  context->addLibFunction(aDivBIntDoubleVectors_func);
  context->addLibFunction(aDivBDoubleIntVectors_func);
  context->addLibFunction(aDivBIntIntVectors_func);
}


int
main (int argc, char* argv[])
{
  int i;
  char *infile=NULL;
  
  test();
  current_function = new FunctionContainer();

  for (i=1; i<argc; i++) {
    if (  strcmp(argv[i], "-debug") == 0  ) {
      // printf("debugging activated (not implemented)\n");
      debug = 1;
    }
    else
      infile = argv[i];
  }

  printf("reading file '%s'\n", infile);
  file = fopen(infile, "r");
  if (  file == NULL  ) {
    printf("cannot open input\n");
    return 12;
    }

  /*================================================================*/
  /*----------------------------------------------------------------*/
  buffer = (char*)malloc(lMaxBuffer);
  if (  buffer == NULL  ) {
    printf("cannot allocate %d bytes of memory\n", lMaxBuffer);
    fclose(file);
    return 12;
  }

  /*================================================================*/
  /* parse it ------------------------------------------------------*/
  DumpRow();
  if (  getNextLine() == 0  )
  yyparse();

  /*================================================================*/
  /* build and run it ------------------------------------------------------*/
  InitializeNativeTarget();
  /* test(); */
  CodeGenContext context;
  addLibraryFunctions(&context);
  context.generateCode(FunctionBlocks);
  context.printLLVMIR();
  context.runCode();
  
  return 0;
}
