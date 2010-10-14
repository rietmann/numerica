%{
  
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "parser.hpp"
/* [-()<>=*+/;{}.\n\^]           { return *yytext; } */
/* void yyerror(char *); */
/* for testing from http://www.ibm.com/developerworks/library/l-lex.html */
%}

chars [A-za-z]
number ([0-9])+
delim [" "\n\t]
whitespace {delim}+
words {chars}+

%%
[0-9]+              { yylval.valInt = atoi(yytext); return INTEGER; }
[0-9]*"."[0-9]+     { yylval.valFloat=atof(yytext); return FLOAT; }
"int" {yylval.typeName = new string(yytext); return VARTYPE;}
"float" {yylval.typeName = new string(yytext); return VARTYPE;}
"void" {yylval.typeName = new string(yytext); return VARTYPE;}
"load" {yylval.langReserve = new string(yytext); return EXT_FUN_LOAD;}
"if" {yylval.langReserve = new string(yytext); return IF;}
">=" {yylval.langReserve = new string(yytext); return GEQ;}
"<=" {yylval.langReserve = new string(yytext); return LEQ;}
[ \t\n]					;
[-()<>=*+/;{}.\^]           { return *yytext; }

{chars}+{number}*{chars}*{number}*       {
  yylval.varName = new string(yytext);
                return VARIABLE;
            }


[ \t]+   ;       /* skip whitespace */

%%
/*[\n]+ ; /*skip newlines*/
/* .               yyerror("Unknown character");  */

int yywrap(void) {
   return 1;
}


/* mode: c */
