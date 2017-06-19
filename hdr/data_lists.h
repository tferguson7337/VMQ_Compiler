#ifndef DATA_LISTS_H_
#define DATA_LISTS_H_

#include "data_rep.h"

struct int_list_node
{
	struct intlit* pil;
	struct int_list_node* next;
};

struct flt_list_node
{
	struct fltlit* pfl;
	struct flt_list_node* next;
};

struct var_list_node
{
	struct varref* pvr;
	struct var_list_node* next;
};

struct str_list_node
{
	struct strlit* psl;
	struct str_list_node* next;
};

typedef struct int_list_node* INT_LIST;
typedef struct flt_list_node* FLT_LIST;
typedef struct var_list_node* VAR_LIST;
typedef struct str_list_node* STR_LIST;

struct func_list_node
{
	int return_type;					// Return type of function, as indicated in cpp file (INT OR FLOAT).
	char* func_name;					// Name of function as found in cpp file.
	VAR_LIST var_list;					// List for tracking func's vars
	unsigned int var_count;				// Tracks number of func's local vars (not params). 
	unsigned int param_count;			// Tracks number of parameters the function takes as an argument.
	unsigned int VMQ_start_line;		// Notes line number of stack frame creation op (#)
	unsigned int VMQ_end_line;			// Notes line number of function end (/, or h if main)
	struct func_list_node* next;
};

typedef struct func_list_node* FUNC_LIST;

/* List Append Functions - Initializes list if list is empty */
struct int_list_node* appendToIntList(INT_LIST* list, char* intval);
struct flt_list_node* appendToFltList(FLT_LIST* list, char* fltval);
struct str_list_node* appendToStrList(STR_LIST* list, char* strval);
struct var_list_node* appendToVarList(VAR_LIST* list, int var_type, char* var_name, int isGlobal, int isParam);
struct func_list_node* appendToFuncList(FUNC_LIST* list, char* func_name);

#endif
