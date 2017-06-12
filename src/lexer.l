/* Lexer for Compiler (C++ ==> VMQ) */
/* Code Author: Thomas Ferguson */

%option noyywrap nodefault yylineno
%{
/*
 * Data structures, #include's, and other things needed
 * by the pattern-match code blocks below go here in here.
*/

#include "parser.tab.h"

char* relops[] = {"<", ">", "<=", ">=", "==", "!="};

%}

/* Basic Operators */
ASSIGNOP ("=")
MULOP ("*"|"/"|"%")
ADDOP ("+"|"-")
INCOP ("+="|"-=")
RELOP ("<"|">"|"<="|">="|"=="|"!=")
NOT ("!")
OR ("||")
AND ("&&")

/* Literals */
INT_LITERAL (0|([1-9][0-9]*))
FLT_LITERAL ({INT_LITERAL}"."[0-9]+)
STR_LITERAL (\"[^\"\n]*\")

/* I/O Operators */
STREAMIN (">>")
STREAMOUT ("<<")

/* Variable/Function Names */
ID ([A-Za-z_][A-Za-z_0-9]*)

/* Keywords */
CIN ("cin")
COUT ("cout")
ELSE ("else")
ENDL ("endl")
FLOAT ("float")
IF ("if")
INT ("int")
RETURN ("return")
WHILE ("while")

/* Misc. */
O_PAREN ("(")
C_PAREN (")")
O_BRACK ("[")
C_BRACK ("]")
O_BRACE ("{")
C_BRACE ("}")
END_STMT (";")
COMMA (",")
LINE_CMNT ("//"[^\n]*)
BLK_CMNT ("/*"[^"*/"]+"*/")
PRE_PROC ("#"[^\n]*)
NEWLINE (\n)

%%

{ASSIGNOP}			{ return ASSIGNOP; }

{MULOP}				{ if(yytext[0] == '*') yylval.op_type = 1; else if (yytext[0] == '/') yylval.op_type = 2; else yylval.op_type = 3; return MULOP; }

{ADDOP}				{ if(yytext[0] == '+') yylval.op_type = 1; else yylval.op_type = 2; return ADDOP; }

{INCOP}				{ if(strcmp(yytext, "+=") == 0) yylval.op_type = 1; else yylval.op_type = 2; return INCOP; }

{RELOP}				{ yylval.op_type = 0; for(int idx = 0; idx < 6; ++idx) { ++(yylval.op_type); if(strcmp(yytext, relops[idx]) == 0) break; } return RELOP; }

{NOT}				{ return NOT; }

{OR}				{ return OR; }

{AND}				{ return AND; }

{FLT_LITERAL}		{ yylval.str = strdup(yytext); return FLT_LITERAL; }

{INT_LITERAL}		{ yylval.str = strdup(yytext); return INT_LITERAL; }

{STR_LITERAL}       { yylval.str = strdup(yytext); return STR_LITERAL; }

{STREAMIN}			{ return STREAMIN; }

{STREAMOUT}			{ return STREAMOUT; }

{CIN}               { return CIN; }

{COUT}              { return COUT; }

{ELSE}              { return ELSE; }

{ENDL}              { return ENDL; }

{FLOAT}             { return FLOAT; }

{IF}                { return IF; }

{INT}               { return INT; }

{RETURN}            { return RETURN; }

{WHILE}             { return WHILE; }

{ID}				{ yylval.str = strdup(yytext); return ID; }

{O_PAREN} |
{C_PAREN} |
{O_BRACK} |
{C_BRACK} |
{O_BRACE} |
{C_BRACE} |
{END_STMT} |
{COMMA}				{ return yytext[0]; }

{LINE_CMNT} |
{BLK_CMNT}	|
{PRE_PROC}			{ }

{NEWLINE}			{ }

[ \t\v]+			{ }

.					{ /* mystery character */ printf("\n\tMYSTERY CHAR %s\n", yytext); }

%%
