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
