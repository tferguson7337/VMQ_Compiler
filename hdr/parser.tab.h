/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_HDR_PARSER_TAB_H_INCLUDED
# define YY_YY_HDR_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ASSIGNOP = 258,
    INCOP = 259,
    ADDOP = 260,
    MULOP = 261,
    NOT = 262,
    OR = 263,
    AND = 264,
    INT_LITERAL = 265,
    FLT_LITERAL = 266,
    STR_LITERAL = 267,
    STREAMIN = 268,
    STREAMOUT = 269,
    ID = 270,
    CIN = 271,
    COUT = 272,
    ELSE = 273,
    ENDL = 274,
    FLOAT = 275,
    IF = 276,
    INT = 277,
    RETURN = 278,
    WHILE = 279,
    VAR_DEC = 280,
    ARR_DEC = 281,
    VAR_ACCESS = 282,
    ARR_ACCESS = 283,
    FUNC_CALL = 284,
    PROG = 285,
    FUNC_DEF = 286,
    FUNC_DEFS = 287,
    ID_LIST = 288,
    VAR_DEF = 289,
    VAR_DEFS = 290,
    TYPE = 291,
    FUNC_HEAD = 292,
    ARGS = 293,
    PARAM_LIST = 294,
    PARAMS = 295,
    BLOCK = 296,
    STMTS = 297,
    STMT = 298,
    INPUT = 299,
    OUTPUT = 300,
    COMP_STMT = 301,
    VAR = 302,
    EXPR_LIST = 303,
    EXPRS = 304,
    EXPR = 305,
    SIMP_EXPR = 306,
    TERM = 307,
    FACTOR = 308,
    LITERAL = 309,
    BOOL_EXPR = 310,
    BOOL_TERM = 311,
    BOOL_FACTOR = 312,
    ADD = 313,
    SUB = 314,
    MUL = 315,
    DIV = 316,
    MOD = 317,
    ADD_ASSIGN = 318,
    SUB_ASSIGN = 319,
    LT = 320,
    GT = 321,
    LTE = 322,
    GTE = 323,
    EQ = 324,
    NEQ = 325,
    RELOP = 326,
    UNARY = 327,
    ADDR = 328
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 21 "src/parser.y" /* yacc.c:1909  */

    struct AST_node* a;		    /* Used to connect nodes of the AST being generated (i.e., used everywhere) */
    int op_type;		    /* Used to distinguish between operators belonging to same category (e.g. RELOP oops) */
    struct intlit* INT_LIT;	    /* Used to reference a literal integer */
    struct fltlit* FLT_LIT;	    /* Used to reference a literal float */
    struct strlit* STR_LIT;	    /* Used to reference a literal string */
    struct var* VAR_REF;		    /* Used to reference a variable */
    struct func_list_node* FUNC;    /* Used to reference a function */

#line 138 "hdr/parser.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HDR_PARSER_TAB_H_INCLUDED  */
