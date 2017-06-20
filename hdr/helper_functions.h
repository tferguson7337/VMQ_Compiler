#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

#include "AST.h"
#include "data_lists.h"
#include "scope.h"

//	================
//	Global Variables
//	================

enum { VMQ_INT_SIZE = 2, VMQ_FLT_SIZE = 4 };

int DEBUG;

/* Lists for tracking global VMQ memory-space elements */
INT_LIST INT_LIST_HEAD;
FLT_LIST FLT_LIST_HEAD;		    // All int/flt/str literals will be stored in VMQ global memory space.
STR_LIST STR_LIST_HEAD;
VAR_LIST GLOBAL_VAR_LIST_HEAD;	    // All variables in C++ global scope will be stored in VMQ global memory space.

/* List for tracking functions as they are encountered in src file */
FUNC_LIST FUNC_LIST_HEAD;

/* Tracks current function (effectively tail of FUNC_LIST_HEAD) */
struct func_list_node* CURRENT_FUNC;

/* Stack for tracking scope (top of stack == current scope) */
SCOPE SCOPE_STACK_HEAD;

/* Tracks the global scope for quickly storing INT, FLT, and STR literals. */
struct scope_stack_node* GLOBAL_SCOPE;

struct AST_node* AST_ROOT;

//	=========
//	Functions
//	=========

/* Initializes global variables (see above) */
void init();

void eval(struct AST_node* a);

#endif
