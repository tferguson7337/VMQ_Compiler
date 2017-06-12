/* Parser for compiler (C++ ==> VMQ) */
/* Code author: Thomas Ferguson */

%{
	#include "parser.tab.h"
	#include "AST.h"
	#include <stdio.h>
	#include <stdlib.h>
	int yylex();
	int yyerror(char *);
/*
 * Data structures, #include's, and other things needed
 * by the pattern-match code blocks below go here in here.
*/

%}

%union
{
	struct ast* a;	/* Used to connect nodes of the AST being generated (i.e., used everywhere) */
	int op_type;	/* Used to distinguish between operators belonging to same category (e.g., RELOP ops) */
	char* str;		/* Used to pass values for FLT_LITERALs, INT_LITERALs, STR_LITERALs, and IDs */
}

/* Operator tokens */
%token ASSIGNOP
%token <op_type> MULOP ADDOP INCOP
%token NOT OR AND

/* Literal tokens */
%token <str> FLT_LITERAL INT_LITERAL STR_LITERAL

/* I/O tokens */
%token STREAMIN STREAMOUT

/* Variable/Function Token */
%token <str> ID

/* Keyword tokens */
%token CIN COUT ELSE ENDL FLOAT IF INT RETURN WHILE

/* Nodetype Tokens - for identifying nodes later in the evaluation stage */
%token PROG FUNC_DEF ID_LIST VAR_DEF TYPE FUNC_HEAD ARGS PARAM_LIST
%token PARAMS BLOCK STMTS STMT INPUT OUTPUT COMP_STMT VAR
%token EXPR_LIST EXPRS EXPR SIMP_EXPR TERM FACTOR LITERAL BOOL_EXPR BOOL_TERM BOOL_FACTOR

/* Precedence and Associativity */

%nonassoc <op_type> RELOP
%right ASSIGNOP INCOP
%left ADDOP
%left MULOP
%nonassoc NOT UNARY

/* Declare type for non-terminals */
%type <a> program function_definitions identifier_list variable_definitions type function_head arguments parameter_list
%type <a> parameters block statements statement input_statement output_statement compound_statement variable
%type <a> expression_list expressions expression simple_expression term factor literal bool_expression bool_term bool_factor

%start program

%%

program:				variable_definitions function_definitions			{ }
;

function_definitions:	function_head block									{ }
					|	function_definitions function_head block			{ }
;

identifier_list:		ID													{ }
					|	ID '[' INT_LITERAL ']'								{ }
					|	identifier_list ',' ID								{ }
					|	identifier_list ',' ID '[' INT_LITERAL ']'			{ }
;

variable_definitions:	/* nothing */										{ }
					|	variable_definitions type identifier_list ';'		{ }
;

type:					INT													{ }
					|	FLOAT												{ }
;

function_head:			type ID arguments									{ }
;

arguments:				'(' parameter_list ')'								{ }
;

parameter_list:			/* nothing */										{ }
					|	parameters											{ }
;

parameters:				type ID												{ }
					|	type ID '[' ']'										{ }
					|	parameters ',' type ID								{ }
					|	parameters ',' type ID '[' ']'						{ }
;

block:					'{' variable_definitions statements '}'				{ }
;

statements:				/* nothing */										{ }
					|	statements statement								{ }
;

statement:				expression ';'										{ }
					|	compound_statement									{ }
					|	RETURN expression ';'								{ }
					|	IF '(' bool_expression ')' statement ELSE statement	{ }
					|	WHILE '(' bool_expression ')' statement				{ }
					|	input_statement ';'									{ }
					|	output_statement ';'								{ }
;

input_statement:		CIN													{ }
					|	input_statement STREAMIN variable					{ }
;

output_statement:		COUT												{ }
					|	output_statement STREAMOUT expression				{ }
					|	output_statement STREAMOUT STR_LITERAL				{ }
					|	output_statement STREAMOUT ENDL						{ }
;


compound_statement:		'{' statements '}'									{ }
;

variable:				ID													{ }
					|	ID '[' expression ']'								{ }
;

expression_list:		/* nothing */										{ }
					|	expressions											{ }
;

expressions:			expression											{ }
					|	expressions ',' expression							{ }
;

expression:				variable ASSIGNOP expression						{ }
					|	variable INCOP expression							{ }
					|	simple_expression									{ }
;

simple_expression:		term												{ }
					|	ADDOP term						%prec UNARY			{ }
					|	simple_expression ADDOP term						{ }
;

term:					factor												{ }
					|	term MULOP factor									{ }
;

factor:					ID													{ }
					|	ID '(' expression_list ')'							{ }
					|	literal												{ }
					|	'(' expression ')'				%prec UNARY			{ }
					|	ID '[' expression ']'								{ }
;

literal:				INT_LITERAL											{ }
					|	FLT_LITERAL											{ }
;

bool_expression:		bool_term											{ }
					|	bool_expression OR bool_term						{ }
;

bool_term:				bool_factor											{ }
					|	bool_term AND bool_factor							{ }
;

bool_factor:			NOT bool_factor										{ }
					|	'(' bool_expression ')'								{ }
					|	simple_expression RELOP simple_expression			{ }
;

%%
