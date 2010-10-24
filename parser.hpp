/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 0,
     FLOAT = 258,
     INTEGER = 259,
     VARIABLE = 260,
     VARTYPE = 261,
     EXT_FUN_LOAD = 262,
     LEQ = 263,
     GEQ = 264,
     IF = 265,
     LBRKT = 266,
     RBRKT = 267,
     FOR = 268,
     FUNCTION = 269,
     RETURN = 270,
     NEG = 271
   };
#endif
/* Tokens.  */
#define END 0
#define FLOAT 258
#define INTEGER 259
#define VARIABLE 260
#define VARTYPE 261
#define EXT_FUN_LOAD 262
#define LEQ 263
#define GEQ 264
#define IF 265
#define LBRKT 266
#define RBRKT 267
#define FOR 268
#define FUNCTION 269
#define RETURN 270
#define NEG 271




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 18 "parser.y"
{
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
}
/* Line 1529 of yacc.c.  */
#line 104 "parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
