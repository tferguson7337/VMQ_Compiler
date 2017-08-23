/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 4 "src/parser.y" /* yacc.c:339  */

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


#line 82 "src/parser.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


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
#line 21 "src/parser.y" /* yacc.c:355  */

    struct AST_node* a;		    /* Used to connect nodes of the AST being generated (i.e., used everywhere) */
    int op_type;		    /* Used to distinguish between operators belonging to same category (e.g. RELOP oops) */
    struct intlit* INT_LIT;	    /* Used to reference a literal integer */
    struct fltlit* FLT_LIT;	    /* Used to reference a literal float */
    struct strlit* STR_LIT;	    /* Used to reference a literal string */
    struct var* VAR_REF;		    /* Used to reference a variable */
    struct func_list_node* FUNC;    /* Used to reference a function */

#line 203 "src/parser.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);



/* Copy the second part of user declarations.  */

#line 220 "src/parser.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   174

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  82
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  133

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      78,    79,     2,     2,    76,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    77,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    74,     2,    75,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,     2,    81,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    84,    84,    87,    88,    91,    92,    94,    95,    99,
     100,   103,   104,   107,   110,   113,   114,   117,   118,   119,
     120,   123,   126,   127,   130,   131,   132,   133,   134,   135,
     136,   139,   140,   143,   144,   145,   146,   150,   153,   154,
     157,   158,   161,   162,   165,   166,   167,   170,   171,   172,
     175,   176,   179,   180,   181,   182,   183,   186,   187,   190,
     191,   194,   195,   198,   199,   200
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ASSIGNOP", "INCOP", "ADDOP", "MULOP",
  "NOT", "OR", "AND", "INT_LITERAL", "FLT_LITERAL", "STR_LITERAL",
  "STREAMIN", "STREAMOUT", "ID", "CIN", "COUT", "ELSE", "ENDL", "FLOAT",
  "IF", "INT", "RETURN", "WHILE", "VAR_DEC", "ARR_DEC", "VAR_ACCESS",
  "ARR_ACCESS", "FUNC_CALL", "PROG", "FUNC_DEF", "FUNC_DEFS", "ID_LIST",
  "VAR_DEF", "VAR_DEFS", "TYPE", "FUNC_HEAD", "ARGS", "PARAM_LIST",
  "PARAMS", "BLOCK", "STMTS", "STMT", "INPUT", "OUTPUT", "COMP_STMT",
  "VAR", "EXPR_LIST", "EXPRS", "EXPR", "SIMP_EXPR", "TERM", "FACTOR",
  "LITERAL", "BOOL_EXPR", "BOOL_TERM", "BOOL_FACTOR", "ADD", "SUB", "MUL",
  "DIV", "MOD", "ADD_ASSIGN", "SUB_ASSIGN", "LT", "GT", "LTE", "GTE", "EQ",
  "NEQ", "RELOP", "UNARY", "ADDR", "'['", "']'", "','", "';'", "'('",
  "')'", "'{'", "'}'", "$accept", "program", "function_definitions",
  "identifier_list", "variable_definitions", "type", "function_head",
  "arguments", "parameter_list", "parameters", "block", "statements",
  "statement", "input_statement", "output_statement", "compound_statement",
  "variable", "expression_list", "expressions", "expression",
  "simple_expression", "term", "factor", "literal", "bool_expression",
  "bool_term", "bool_factor", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,    91,    93,    44,    59,    40,    41,
     123,   125
};
# endif

#define YYPACT_NINF -80

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-80)))

#define YYTABLE_NINF -40

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -80,    14,    19,   -80,   -80,   -80,    19,     2,   -48,    31,
     -48,    -9,    32,   -80,   -80,    34,   -80,    92,    19,   -80,
      95,   -80,    19,    39,   101,    41,    46,    50,   111,    -5,
     -80,    56,   -80,    19,   121,    59,    82,   -80,   -80,     0,
     -80,   -80,    60,    45,    61,    45,   -80,   -80,   -80,   -11,
       9,   -80,   115,    57,   132,   134,   -80,   -80,    66,   127,
      68,     7,   134,    45,    45,    72,    67,    72,    69,    10,
     130,   -80,    25,   -80,    45,    45,   -80,    82,    82,   -80,
      73,   -80,    45,    71,    70,    75,   -80,    72,    89,    24,
      -7,   143,   -80,   -80,     1,   -80,   -80,    79,   -80,   -80,
     -80,   -80,   -80,   -80,   134,   -80,    80,    81,   125,   -80,
      45,   -80,    24,     5,    96,    72,    47,    72,    47,    45,
     -80,   -80,   -80,   -80,   132,   143,   136,   -80,   -80,    83,
      47,   -80,   -80
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       9,     0,     0,     1,    12,    11,     2,     0,     0,     0,
       0,     5,     0,     9,     3,     0,     4,     0,    15,    13,
       0,    10,    22,     0,     0,     0,    16,     7,     0,     0,
       6,    17,    14,     0,     0,     5,     0,    57,    58,    52,
      31,    33,     0,     0,     0,     0,    22,    21,    23,     0,
       0,    25,     0,     0,    46,    47,    50,    54,     0,     0,
       0,    52,    48,     0,    40,     0,     0,     0,     0,     0,
       0,    29,     0,    30,     0,     0,    24,     0,     0,    18,
      19,     8,     0,     0,     0,    41,    42,     0,     0,     0,
       0,    59,    61,    26,     0,    55,    37,    38,    32,    35,
      36,    34,    44,    45,    49,    51,     0,     0,    56,    53,
       0,    63,    46,     0,     0,     0,     0,     0,     0,     0,
      20,    56,    43,    64,    65,    60,     0,    62,    28,     0,
       0,    39,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -80,   -80,   -80,   -80,   144,    99,   153,   -80,   -80,   -80,
     151,   118,   -71,   -80,   -80,   -80,    98,   -80,   -80,   -21,
      48,   -29,    88,   -80,   -39,    54,   -79
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     6,    12,     2,     7,     8,    19,    25,    26,
      14,    29,    48,    49,    50,    51,    52,    84,    85,    53,
      54,    55,    56,    57,    90,    91,    92
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      36,   115,    70,   -38,   -38,    37,    38,    62,   111,   115,
      39,    40,    41,   115,     3,    36,    42,    11,    43,    44,
      37,    38,    66,    72,    68,    39,    40,    41,    94,    77,
      36,    42,    13,    43,    44,    37,    38,    99,   127,     4,
      39,     5,    83,    86,   100,   126,    15,   128,   104,   113,
      36,   101,    36,   102,   103,    37,    38,    37,    38,   132,
      39,   107,    39,    40,    41,    17,    71,    68,    42,    18,
      43,    44,   116,    45,    63,    46,    47,    36,    64,    87,
     118,    82,    37,    38,   123,    64,    73,    61,    45,   122,
      46,    96,    37,    38,    36,   114,    87,    61,   129,    37,
      38,    36,    23,    45,    39,     9,    37,    38,    20,    21,
      27,    61,    18,    89,    30,    89,    31,    24,    74,    75,
      32,    28,    33,    45,    34,    45,    35,    46,   -39,   -39,
      58,    60,    59,    17,    76,    89,   112,    77,    65,    67,
      78,    79,    80,    81,    93,    97,   108,   106,    95,   109,
      88,   110,   117,   119,   130,   120,   121,    22,   131,    10,
      45,    16,   124,    89,    69,    89,   105,    88,    98,   125,
       0,     0,     0,     0,    45
};

static const yytype_int16 yycheck[] =
{
       5,     8,    13,     3,     4,    10,    11,    36,    87,     8,
      15,    16,    17,     8,     0,     5,    21,    15,    23,    24,
      10,    11,    43,    14,    45,    15,    16,    17,    67,     5,
       5,    21,    80,    23,    24,    10,    11,    12,   117,    20,
      15,    22,    63,    64,    19,   116,    15,   118,    77,    88,
       5,    72,     5,    74,    75,    10,    11,    10,    11,   130,
      15,    82,    15,    16,    17,    74,    77,    88,    21,    78,
      23,    24,    79,    78,    74,    80,    81,     5,    78,     7,
      79,    74,    10,    11,    79,    78,    77,    15,    78,   110,
      80,    81,    10,    11,     5,    71,     7,    15,   119,    10,
      11,     5,    10,    78,    15,     6,    10,    11,    76,    77,
      15,    15,    78,    65,    75,    67,    15,    18,     3,     4,
      79,    22,    76,    78,    74,    78,    15,    80,     3,     4,
      74,    10,    33,    74,    77,    87,    88,     5,    78,    78,
       6,    75,    15,    75,    77,    15,    75,    74,    79,    79,
      78,    76,     9,    74,    18,    75,    75,    13,    75,     6,
      78,    10,   114,   115,    46,   117,    78,    78,    70,   115,
      -1,    -1,    -1,    -1,    78
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    83,    86,     0,    20,    22,    84,    87,    88,    87,
      88,    15,    85,    80,    92,    15,    92,    74,    78,    89,
      76,    77,    86,    10,    87,    90,    91,    15,    87,    93,
      75,    15,    79,    76,    74,    15,     5,    10,    11,    15,
      16,    17,    21,    23,    24,    78,    80,    81,    94,    95,
      96,    97,    98,   101,   102,   103,   104,   105,    74,    87,
      10,    15,   103,    74,    78,    78,   101,    78,   101,    93,
      13,    77,    14,    77,     3,     4,    77,     5,     6,    75,
      15,    75,    74,   101,    99,   100,   101,     7,    78,   102,
     106,   107,   108,    77,   106,    79,    81,    15,    98,    12,
      19,   101,   101,   101,   103,   104,    74,   101,    75,    79,
      76,   108,   102,   106,    71,     8,    79,     9,    79,    74,
      75,    75,   101,    79,   102,   107,    94,   108,    94,   101,
      18,    75,    94
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    82,    83,    84,    84,    85,    85,    85,    85,    86,
      86,    87,    87,    88,    89,    90,    90,    91,    91,    91,
      91,    92,    93,    93,    94,    94,    94,    94,    94,    94,
      94,    95,    95,    96,    96,    96,    96,    97,    98,    98,
      99,    99,   100,   100,   101,   101,   101,   102,   102,   102,
     103,   103,   104,   104,   104,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   108
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     3,     1,     4,     3,     6,     0,
       4,     1,     1,     3,     3,     0,     1,     2,     4,     4,
       6,     4,     0,     2,     2,     1,     3,     7,     5,     2,
       2,     1,     3,     1,     3,     3,     3,     3,     1,     4,
       0,     1,     1,     3,     3,     3,     1,     1,     2,     3,
       1,     3,     1,     4,     1,     3,     4,     1,     1,     1,
       3,     1,     3,     2,     3,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 84 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = AST_ROOT = create_AST_node(PROG, (yyvsp[-1].a), (yyvsp[0].a)); }
#line 1411 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 87 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(FUNC_DEF, (yyvsp[-1].a), (yyvsp[0].a)); }
#line 1417 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 88 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(FUNC_DEFS, (yyvsp[-2].a), create_AST_node(FUNC_DEF, (yyvsp[-1].a), (yyvsp[0].a))); }
#line 1423 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 91 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_var_node(VAR_DEC, (yyvsp[0].VAR_REF)); }
#line 1429 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 92 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ARR_DEC, create_var_node(ID, (yyvsp[-3].VAR_REF)), create_int_node(INT_LITERAL, (yyvsp[-1].INT_LIT))); 
										    (yyvsp[-3].VAR_REF)->size = atoi((yyvsp[-1].INT_LIT)->val); if(atoi((yyvsp[-1].INT_LIT)->val) <= 0) yyerror("INVALID ARRAY SIZE"); }
#line 1436 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 94 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ID_LIST, (yyvsp[-2].a), create_var_node(VAR_DEC, (yyvsp[0].VAR_REF))); }
#line 1442 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 95 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ID_LIST, (yyvsp[-5].a), create_AST_node(ARR_DEC, create_var_node(ID, (yyvsp[-3].VAR_REF)), create_int_node(INT_LITERAL, (yyvsp[-1].INT_LIT))));
										    (yyvsp[-3].VAR_REF)->size = atoi((yyvsp[-1].INT_LIT)->val); if(atoi((yyvsp[-1].INT_LIT)->val) <= 0) yyerror("INVALID ARRAY SIZE"); }
#line 1449 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 99 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1455 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 100 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(VAR_DEFS, (yyvsp[-3].a), create_AST_node((yyvsp[-2].op_type), (yyvsp[-1].a), NULL)); }
#line 1461 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 103 "src/parser.y" /* yacc.c:1646  */
    { (yyval.op_type) = INT; }
#line 1467 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 104 "src/parser.y" /* yacc.c:1646  */
    { (yyval.op_type) = FLOAT; }
#line 1473 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 107 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(FUNC_HEAD, create_func_node(ID, yylval.FUNC), (yyvsp[0].a)); }
#line 1479 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 110 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[-1].a); }
#line 1485 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 113 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1491 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 114 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(PARAM_LIST, (yyvsp[0].a), NULL); }
#line 1497 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 117 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_var_node(ID, (yyvsp[0].VAR_REF)); }
#line 1503 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 118 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_var_node(ID, (yyvsp[-2].VAR_REF)); }
#line 1509 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 119 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(PARAMS, (yyvsp[-3].a), create_var_node(ID, (yyvsp[0].VAR_REF))); }
#line 1515 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 120 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(PARAMS, (yyvsp[-5].a), create_var_node(ID, (yyvsp[-2].VAR_REF))); }
#line 1521 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 123 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(BLOCK, (yyvsp[-2].a), (yyvsp[-1].a)); }
#line 1527 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 126 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1533 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 127 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STMTS, (yyvsp[-1].a), (yyvsp[0].a)); }
#line 1539 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 130 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STMT, (yyvsp[-1].a), NULL); }
#line 1545 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 131 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STMT, (yyvsp[0].a), NULL); }
#line 1551 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 132 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(RETURN, (yyvsp[-1].a), NULL); }
#line 1557 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 133 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_ctrl_node(IF, (yyvsp[-4].a), (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1563 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 134 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_ctrl_node(WHILE, (yyvsp[-2].a), (yyvsp[0].a), NULL); }
#line 1569 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 135 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(INPUT, (yyvsp[-1].a), NULL); }
#line 1575 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 136 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(OUTPUT, (yyvsp[-1].a), NULL); }
#line 1581 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 139 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1587 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 140 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STREAMIN, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1593 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 143 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1599 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 144 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STREAMOUT, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1605 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 145 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STREAMOUT, (yyvsp[-2].a), create_str_node(STR_LITERAL, (yyvsp[0].STR_LIT))); }
#line 1611 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 146 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(STREAMOUT, (yyvsp[-2].a), create_str_node(ENDL, (yyvsp[0].STR_LIT))); }
#line 1617 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 150 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[-1].a); }
#line 1623 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 153 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_var_node(VAR_ACCESS, (yyvsp[0].VAR_REF)); }
#line 1629 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 154 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ARR_ACCESS, create_var_node(ID, (yyvsp[-3].VAR_REF)), (yyvsp[-1].a)); }
#line 1635 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 157 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(0, NULL, NULL); }
#line 1641 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 158 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 1647 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 161 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 1653 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 162 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(EXPRS, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1659 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 165 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ASSIGNOP, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1665 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 166 "src/parser.y" /* yacc.c:1646  */
    { if((yyvsp[-1].op_type) == 1) (yyval.a) = create_AST_node(ADD_ASSIGN, (yyvsp[-2].a), (yyvsp[0].a)); else (yyval.a) = create_AST_node(SUB_ASSIGN, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1671 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 167 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 1677 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 170 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a);}
#line 1683 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 171 "src/parser.y" /* yacc.c:1646  */
    { if((yyvsp[-1].op_type) == 1) (yyval.a) = create_AST_node(ADD, (yyvsp[0].a), NULL); else (yyval.a) = create_AST_node(SUB, (yyvsp[0].a), NULL); }
#line 1689 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 172 "src/parser.y" /* yacc.c:1646  */
    { if((yyvsp[-1].op_type) == 1) (yyval.a) = create_AST_node(ADD, (yyvsp[-2].a), (yyvsp[0].a)); else (yyval.a) = create_AST_node(SUB, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1695 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 175 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 1701 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 176 "src/parser.y" /* yacc.c:1646  */
    { int op; if((yyvsp[-1].op_type) == 1) op = MUL; else if ((yyvsp[-1].op_type) == 2) op = DIV; else op = MOD; (yyval.a) = create_AST_node(op, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1707 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 179 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_var_node(VAR_ACCESS, (yyvsp[0].VAR_REF)); }
#line 1713 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 180 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(FUNC_CALL, create_func_node(ID, (struct func_list_node*)(yyvsp[-3].VAR_REF)), (yyvsp[-1].a)); }
#line 1719 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 181 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 1725 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 182 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[-1].a); }
#line 1731 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 183 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(ARR_ACCESS, create_var_node(ID, (yyvsp[-3].VAR_REF)), (yyvsp[-1].a)); }
#line 1737 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 186 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_int_node(INT_LITERAL, (yyvsp[0].INT_LIT)); }
#line 1743 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 187 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_flt_node(FLT_LITERAL, (yyvsp[0].FLT_LIT)); }
#line 1749 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 190 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a);}
#line 1755 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 191 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_logic_node(OR, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1761 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 194 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a);}
#line 1767 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 195 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_logic_node(AND, (yyvsp[-2].a), (yyvsp[0].a)); }
#line 1773 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 198 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = create_AST_node(NOT, (yyvsp[0].a), NULL); }
#line 1779 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 199 "src/parser.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[-1].a); }
#line 1785 "src/parser.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 200 "src/parser.y" /* yacc.c:1646  */
    { 
										    int op; 
										    switch((yyvsp[-1].op_type))
										    {
											case 1: op = LT; break;
											case 2: op = GT; break;
											case 3: op = LTE; break;
											case 4: op = GTE; break;
											case 5: op = EQ; break;
											case 6: op = NEQ; break;
											default: yyerror("\n\tParser: Unknown RELOP value encountered!\n"); exit(-1);
										    }
										    
										    (yyval.a) = create_relop_node(op, (yyvsp[-2].a), (yyvsp[0].a));
										}
#line 1805 "src/parser.tab.c" /* yacc.c:1646  */
    break;


#line 1809 "src/parser.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 217 "src/parser.y" /* yacc.c:1906  */

