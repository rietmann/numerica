%option noyywrap

%{
  
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "parser.hpp"
  
 #define YY_INPUT(buf,result,max_size)  {\
    result = GetNextChar(buf, max_size); \
  if (  result <= 0  )	{			\
    result = YY_NULL; } }			
  
/* [-()<>=*+/;{}.\n\^]           { return *yytext; } */
/* void yyerror(char *); */
/* for testing from http://www.ibm.com/developerworks/library/l-lex.html */
/* "["  {yylval.langReserve = new string(yytext); return LBRKT;} */
/* "]"  {yylval.langReserve = new string(yytext); return RBRKT;} */
%}

chars [A-za-z]
number ([0-9])+
delim [" "\n\t]
whitespace {delim}+
words {chars}+

%%
[ \t\r\n]+  {
    /* eat up whitespace */
    BeginToken(yytext); 
    }

[0-9]+              { BeginToken(yytext); yylval.valInt = atoi(yytext); return INTEGER; }
[0-9]*"."[0-9]+     { yylval.valFloat=atof(yytext); return FLOAT; }
"int" {yylval.typeName = new string(yytext); return VARTYPE;}
"float" {yylval.typeName = new string(yytext); return VARTYPE;}
"void" {yylval.typeName = new string(yytext); return VARTYPE;}
"int[]" {yylval.typeName = new string(yytext); return VARTYPE;}
"float[]" {yylval.typeName = new string(yytext); return VARTYPE;}
"load" {yylval.langReserve = new string(yytext); return EXT_FUN_LOAD;}
"function" {yylval.langReserve = new string(yytext); return FUNCTION;}
"return" {yylval.langReserve = new string(yytext); return RETURN;}
"for" {yylval.langReserve = new string(yytext); return FOR;}
"if" {yylval.langReserve = new string(yytext); return IF;}
">=" {yylval.langReserve = new string(yytext); return GEQ;}
"<=" {yylval.langReserve = new string(yytext); return LEQ;}

[-()<>=*+/:;,\[\]{}\^.]           { return *yytext; }

[_a-zA-Z][_a-zA-Z0-9]*  {
  yylval.varName = new string(yytext);
                return VARIABLE;
            }

.         { BeginToken(yytext); return yytext[0]; }

%%

/* [\[\]]                  { printf("Found %s\n",yytext); return *yytext;} */
/*[\n]+ ; /*skip newlines*/
/* .               yyerror("Unknown character");  */

/* int yywrap(void) { */
/*    return 1; */
/* } */


/* mode: c */
