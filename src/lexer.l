/* Lexer for Compiler (C++ ==> VMQ) */
/* Code Author: Thomas Ferguson */

%option noyywrap nodefault yylineno
%{
/*
 * Data structures, #include's, and other things needed
 * by the pattern-match code blocks below go here in here.
*/
#include "helper_functions.h"
#include "error_handling.h"
#include "parser.tab.h"

/* Var/Func Definition Flags */
int definingInt = 0;
int definingFlt = 0;

char* relops[] = {"<", ">", "<=", ">=", "==", "!="};
int line_count = 1;

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

{ASSIGNOP}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return ASSIGNOP; }

{MULOP}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  if(yytext[0] == '*') yylval.op_type = 1; else if (yytext[0] == '/') yylval.op_type = 2; else yylval.op_type = 3; return MULOP; }

{ADDOP}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  if(yytext[0] == '+') yylval.op_type = 1; else yylval.op_type = 2; return ADDOP; }

{INCOP}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  if(strcmp(yytext, "+=") == 0) yylval.op_type = 1; else yylval.op_type = 2; return INCOP; }

{RELOP}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  yylval.op_type = 0; for(int idx = 0; idx < 6; ++idx) { ++(yylval.op_type); if(strcmp(yytext, relops[idx]) == 0) break; } return RELOP; }

{NOT}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return NOT; }

{OR}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return OR; }

{AND}	            { if(DEBUG) printf("%s", yytext); fflush(stdout);  return AND; }

{FLT_LITERAL}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  yylval.FLT_LIT = (appendToFltList(yytext))->pfl; return FLT_LITERAL; }

{INT_LITERAL}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  yylval.INT_LIT = (appendToIntList(yytext))->pil; return INT_LITERAL; }

{STR_LITERAL}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  yylval.STR_LIT = (appendToStrList(yytext))->psl; return STR_LITERAL; }

{STREAMIN}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return STREAMIN; }

{STREAMOUT}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return STREAMOUT; }

{CIN}               { if(DEBUG) printf("%s", yytext); fflush(stdout);  return CIN; }

{COUT}              { if(DEBUG) printf("%s", yytext); fflush(stdout);  return COUT; }

{ELSE}              { if(DEBUG) printf("%s", yytext); fflush(stdout);  return ELSE; }

{ENDL}              { if(DEBUG) printf("%s", yytext); fflush(stdout);  yylval.STR_LIT = (appendToStrList("\\n"))->psl; return ENDL; }

{FLOAT}             { if(DEBUG) printf("%s", yytext); fflush(stdout);  definingFlt = 1; yylval.op_type = FLOAT; return FLOAT; }

{IF}/([ \t]*"(")    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return IF; }

{INT}               { if(DEBUG) printf("%s", yytext); fflush(stdout);  definingInt = 1; yylval.op_type = INT; return INT; }

{RETURN}            { if(DEBUG) printf("%s", yytext); fflush(stdout);  return RETURN; }

{WHILE}/([ \t]*"(") { if(DEBUG) printf("%s", yytext); fflush(stdout);  return WHILE; }

{ID}		    {
			if(DEBUG) printf("%s", yytext); fflush(stdout);  
			if(definingInt || definingFlt)
			{
			    if(symbolTableLookup(yytext))
				yyerror("Variable redefinition");
			    else
				addToSymbolTable(&(SCOPE_STACK_HEAD->symTab), yytext);
			    
			    int data_type;
			    if(definingInt)
				data_type = INT;
			    else
				data_type = FLOAT;

			    yylval.VAR = appendToVarList(data_type, yytext)->pv;
			    
			    return ID;
			}
			else // Referencing variable (not defining)
			{
			    if(!symbolTableLookup(yytext))
				yyerror("Using undefined variable");
			    
			    // Variable exists - check global list first.
			    struct var_list_node* varListPtr = GLOBAL_VAR_LIST_HEAD;
			    while(varListPtr)
			    {
				if(strcmp(varListPtr->pv->var_name, yytext) == 0)
				{
				    yylval.VAR = varListPtr->pv;
				    return ID;
				}
				varListPtr = varListPtr->next;
			    }

			    // Var isn't found yet, check the local variable list.
			    varListPtr = CURRENT_FUNC->var_list_head;
			    while(varListPtr)
			    {
				if(strcmp(varListPtr->pv->var_name, yytext) == 0)
				{
				    yylval.VAR = varListPtr->pv;
				    return ID;
				}
				varListPtr = varListPtr->next;
			    }

			    // Var still isn't found; check the params list.
			    varListPtr = CURRENT_FUNC->param_list_head;
			    while(varListPtr)
			    {
				if(strcmp(varListPtr->pv->var_name, yytext) == 0)
				{
				    yylval.VAR = varListPtr->pv;
				    return ID;
				}
				varListPtr = varListPtr->next;
			    }

			    yyerror("Variable reference not found in global, local, or parameter list!");
			}
		    }

{ID}/([ \t\v]*"(")  {	
			if(DEBUG) printf("%s", yytext); fflush(stdout);  
			if(definingInt || definingFlt)
			{   // Function definition
			    if(symbolTableLookup(yytext)) // This approach won't allow function overloading.
				yyerror("Function name already taken (redefinition with equivalent ID)");
			    else
				addToSymbolTable(&(SCOPE_STACK_HEAD->symTab), yytext);
			    pushScope();
			    if(definingInt)
				yylval.FUNC = appendToFuncList(INT, yytext);
			    else
				yylval.FUNC = appendToFuncList(FLOAT, yytext);
			}
			else // Function call
			{ 
			    if(!symbolTableLookup(yytext))
			    {
				char* err_msg = malloc(64);
				sprintf(err_msg, "Function name \"%s\" not found", yytext);
				yyerror(err_msg);
				free(err_msg);
			    }
			    struct func_list_node* funcPtr = FUNC_LIST_HEAD;
			    while(funcPtr)
			    {
				if(strcmp(funcPtr->func_name, yytext) == 0)
				{
				    yylval.FUNC = funcPtr;
				    break;
				}
				funcPtr = funcPtr->next;
			    }

			    if(!funcPtr)
				yyerror("Function not found in function list!");
			}

			return ID; 
		    }

{O_PAREN}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return yytext[0]; }
{C_PAREN} 	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  definingInt = definingFlt = 0; return yytext[0]; }
{O_BRACK} 	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return yytext[0]; }
{C_BRACK} 	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return yytext[0]; }
{O_BRACE}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  if(!(SCOPE_STACK_HEAD->isNewScope)) pushScope(); SCOPE_STACK_HEAD->isNewScope = 0; return yytext[0]; }
{C_BRACE} 	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  popScope(); return yytext[0]; }
{END_STMT} 	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  definingInt = definingFlt = 0; return yytext[0]; }
{COMMA}		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  return yytext[0]; }

{LINE_CMNT} |
{BLK_CMNT}  |
{PRE_PROC}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  /* Do nothing */  }

{NEWLINE}	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  }

[ \t\v]+	    { if(DEBUG) printf("%s", yytext); fflush(stdout);  /* Do nothing */  }

.		    { if(DEBUG) printf("%s", yytext); fflush(stdout);  /* mystery character */ printf("\n\tMYSTERY CHAR %s\n", yytext); fflush(stdout);  }

%%
