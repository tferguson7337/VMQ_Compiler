/* Parser for compiler (C++ ==> VMQ) */
/* Code author: Thomas Ferguson */

%{
    #include "parser.tab.h"
    #include "helper_functions.h"
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
    struct AST_node* a;		    /* Used to connect nodes of the AST being generated (i.e., used everywhere) */
    int op_type;		    /* Used to distinguish between operators belonging to same category (e.g. RELOP oops) */
    struct intlit* INT_LIT;	    /* Used to reference a literal integer */
    struct fltlit* FLT_LIT;	    /* Used to reference a literal float */
    struct strlit* STR_LIT;	    /* Used to reference a literal string */
    struct varref* VAR_REF;	    /* Used to reference a variable */
    struct func_list_node* FUNC;    /* Used to reference a function */
}

/* Operator tokens */
%token ASSIGNOP
%token <op_type> MULOP ADDOP INCOP
%token NOT OR AND

/* Literal tokens */
%token <INT_LIT> INT_LITERAL 
%token <FLT_LIT> FLT_LITERAL
%token <STR_LIT> STR_LITERAL

/* I/O tokens */
%token STREAMIN STREAMOUT

/* Variable/Function Token */
%token <VAR_REF> ID

/* Keyword tokens */
%token CIN COUT ELSE 
%token <STR_LIT> ENDL 
%token FLOAT IF INT RETURN WHILE

/* Nodetype Tokens - for identifying nodes later in the evaluation stage */
%token VAR_DEC ARR_DEC VAR_ACCESS ARR_ACCESS FUNC_CALL

%token PROG FUNC_DEF FUNC_DEFS ID_LIST VAR_DEF VAR_DEFS TYPE FUNC_HEAD ARGS PARAM_LIST
%token PARAMS BLOCK STMTS STMT INPUT OUTPUT COMP_STMT VAR
%token EXPR_LIST EXPRS EXPR SIMP_EXPR TERM FACTOR LITERAL BOOL_EXPR BOOL_TERM BOOL_FACTOR

%token ADD SUB MUL DIV MOD ADD_ASSIGN SUB_ASSIGN LT GT LTE GTE EQ NEQ

/* Precedence and Associativity */

%nonassoc <op_type> RELOP
%right ASSIGNOP INCOP
%left ADDOP
%left MULOP
%nonassoc NOT UNARY

/* Declare type for non-terminals */
%type <a> program function_definitions identifier_list variable_definitions function_head arguments parameter_list
%type <a> parameters block statements statement input_statement output_statement compound_statement variable
%type <a> expression_list expressions expression simple_expression term factor literal bool_expression bool_term bool_factor

%type <op_type> type // Type returns variable type, which is represented as an int.

%start program

%%

program:		variable_definitions function_definitions		{ $$ = AST_ROOT = create_AST_node(PROG, $1, $2); }
;

function_definitions:	function_head block					{ $$ = create_AST_node(FUNC_DEF, $1, $2); }
		|	function_definitions function_head block		{ $$ = create_AST_node(FUNC_DEFS, $1, create_AST_node(FUNC_DEF, $2, $3)); }
;

identifier_list:	ID							{ $$ = create_var_node(VAR_DEC, $1); }
		    |   ID '[' INT_LITERAL ']'					{ $$ = create_AST_node(ARR_DEC, create_var_node(ID, $1), create_int_node(INT_LITERAL, $3)); 
										    $1->val->size = atoi($3->val); if(atoi($3->val) <= 0) { yyerror("INVALID ARRAY SIZE"); exit(-1); } }
		    |   identifier_list ',' ID					{ $$ = create_AST_node(ID_LIST, $1, create_var_node(VAR_DEC, $3)); }
		    |   identifier_list ',' ID '[' INT_LITERAL ']'		{ $$ = create_AST_node(ID_LIST, $1, create_AST_node(ARR_DEC, create_var_node(ID, $3), create_int_node(INT_LITERAL, $5)));
										    $3->val->size = atoi($5->val); if(atoi($5->val) <= 0) { yyerror("INVALID ARRAY SIZE"); exit(-1); } }
;

variable_definitions:	/* nothing */						{ $$ = create_AST_node(0, NULL, NULL); }
		    |	variable_definitions type identifier_list ';'		{ $$ = create_AST_node(VAR_DEFS, $1, create_AST_node($2, $3, NULL)); }
;

type:			INT							{ $$ = INT; }
		    |	FLOAT							{ $$ = FLOAT; }
;

function_head:		type ID arguments					{ $$ = create_AST_node(FUNC_HEAD, create_var_node(ID, $2), $3); }
;

arguments:		'(' parameter_list ')'					{ $$ = $2; }
;

parameter_list:		/* nothing */						{ $$ = create_AST_node(0, NULL, NULL); }
		    |	parameters						{ $$ = create_AST_node(PARAM_LIST, $1, NULL); }
;

parameters:		type ID							{ $$ = create_var_node(ID, $2); }
		    |	type ID '[' ']'						{ $$ = create_var_node(ID, $2); }
		    |	parameters ',' type ID					{ $$ = create_AST_node(PARAMS, $1, create_var_node(ID, $4)); }
		    |	parameters ',' type ID '[' ']'				{ $$ = create_AST_node(PARAMS, $1, create_var_node(ID, $4)); }
;

block:			'{' variable_definitions statements '}'			{ $$ = create_AST_node(BLOCK, $2, $3); }
;

statements:		/* nothing */						{ $$ = create_AST_node(0, NULL, NULL); }
		    |	statements statement					{ $$ = create_AST_node(STMTS, $1, $2); }
;

statement:		expression ';'						{ $$ = create_AST_node(STMT, $1, NULL); }
		    |	compound_statement					{ $$ = create_AST_node(STMT, $1, NULL); }
		    |	RETURN expression ';'					{ $$ = create_AST_node(RETURN, $2, NULL); }
		    |	IF '(' bool_expression ')' statement ELSE statement	{ $$ = create_ctrl_node(IF, $3, $5, $7); }
		    |	WHILE '(' bool_expression ')' statement			{ $$ = create_ctrl_node(WHILE, $3, $5, NULL); }
		    |	input_statement ';'					{ $$ = create_AST_node(INPUT, $1, NULL); }
		    |	output_statement ';'					{ $$ = create_AST_node(OUTPUT, $1, NULL); }
;

input_statement:	CIN							{ $$ = create_AST_node(0, NULL, NULL); }
		    |	input_statement STREAMIN variable			{ $$ = create_AST_node(STREAMIN, $1, $3); }
;

output_statement:	COUT							{ $$ = create_AST_node(0, NULL, NULL); }
		    |	output_statement STREAMOUT expression			{ $$ = create_AST_node(STREAMOUT, $1, $3); }
		    |	output_statement STREAMOUT STR_LITERAL			{ $$ = create_AST_node(STREAMOUT, $1, create_str_node(STR_LITERAL, $3)); }
		    |	output_statement STREAMOUT ENDL				{ $$ = create_AST_node(STREAMOUT, $1, create_str_node(ENDL, $3)); }
;


compound_statement:	'{' statements '}'					{ $$ = $2; }
;

variable:		ID							{ $$ = create_var_node(VAR_ACCESS, $1); }
		    |	ID '[' expression ']'					{ $$ = create_AST_node(ARR_ACCESS, create_var_node(ID, $1), $3); }
;

expression_list:	/* nothing */						{ $$ = create_AST_node(0, NULL, NULL); }
		    |	expressions						{ $$ = $1; }
;

expressions:		expression						{ $$ = $1; }
		    |	expressions ',' expression				{ $$ = create_AST_node(EXPRS, $1, $3); }
;

expression:		variable ASSIGNOP expression				{ $$ = create_AST_node(ASSIGNOP, $1, $3); }
		    |	variable INCOP expression				{ if($2 == 1) $$ = create_AST_node(ADD_ASSIGN, $1, $3); else $$ = create_AST_node(SUB_ASSIGN, $1, $3); }
		    |	simple_expression					{ $$ = $1; }
;

simple_expression:	term							{ $$ = $1;}
		    |	ADDOP term			%prec UNARY		{ if($1 == 1) $$ = create_AST_node(ADD, $2, NULL); else $$ = create_AST_node(SUB, $2, NULL); }
		    |	simple_expression ADDOP term				{ if($2 == 1) $$ = create_AST_node(ADD, $1, $3); else $$ = create_AST_node(SUB, $1, $3); }
;

term:			factor							{ $$ = $1; }
		    |	term MULOP factor					{ int op; if($2 == 1) op = MUL; else if ($2 == 2) op = DIV; else op = MOD; create_AST_node(op, $1, $3); }
;

factor:			ID							{ $$ = create_var_node(VAR_ACCESS, $1); }
		    |	ID '(' expression_list ')'				{ $$ = create_AST_node(FUNC_CALL, create_var_node(ID, $1), $3); }
		    |	literal							{ $$ = $1; }
		    |	'(' expression ')'		%prec UNARY		{ $$ = $2; }
		    |	ID '[' expression ']'					{ $$ = create_AST_node(ARR_ACCESS, create_var_node(ID, $1), $3); }
;

literal:		INT_LITERAL						{ $$ = create_int_node(INT_LITERAL, $1); }
		    |	FLT_LITERAL						{ $$ = create_flt_node(FLT_LITERAL, $1); }
;

bool_expression:	bool_term						{ $$ = $1;}
		    |	bool_expression OR bool_term				{ $$ = create_AST_node(OR, $1, $3); }
;

bool_term:		bool_factor						{ $$ = $1;}
		    |	bool_term AND bool_factor				{ $$ = create_AST_node(AND, $1, $3); }
;

bool_factor:		NOT bool_factor						{ $$ = create_AST_node(NOT, NULL, $2); }
		    |	'(' bool_expression ')'					{ $$ = $2; }
		    |	simple_expression RELOP simple_expression		{ 
										    int op; 
										    switch($2)
										    {
											case 1: op = LT; break;
											case 2: op = GT; break;
											case 3: op = LTE; break;
											case 4: op = GTE; break;
											case 5: op = EQ; break;
											case 6: op = NEQ; break;
											default: yyerror("\n\tParser: Unknown RELOP value encountered!\n"); exit(-1);
										    }
										    
										    $$ = create_AST_node(op, $1, $3);
										}
;

%%
