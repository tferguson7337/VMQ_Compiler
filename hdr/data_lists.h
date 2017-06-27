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

struct VMQ_list_node
{
    char* VMQ_line;
    struct VMQ_list_node* next;
};

// In VMQ, temporary variables must be used to stored the
// results of intermediate calculations.
struct VMQ_temp_node
{
    unsigned int VMQ_loc;
    unsigned int data_type;
    struct VMQ_temp_node* next;
};

struct VMQ_mem_node
{
    unsigned int nodetype;
    void* node;
    struct VMQ_mem_node* next;
};

typedef struct int_list_node* INT_LIST;
typedef struct flt_list_node* FLT_LIST;
typedef struct var_list_node* VAR_LIST;
typedef struct str_list_node* STR_LIST;
typedef struct VMQ_list_node* VMQ_LIST;
typedef struct VMQ_temp_node* TEMP_LIST;
typedef struct VMQ_mem_node*  VMQ_MEM_LIST;

struct VMQ_func_data
{
    VMQ_LIST stmt_list_head;		// List for storing a C++ function's equivalent VMQ statements
    VMQ_LIST stmt_list_tail;		// Pointer to the VMQ_LIST's last node, convenient for insert
    unsigned int stmt_count;		// Counter for number of elements in VMQ_LIST VMQ_list_head
    unsigned int quad_start_line;	// Quad addr that marks beginning stmt of function
    unsigned int quad_end_line;		// Quad addr that marks end stmt of function
    VAR_LIST temp_list_head;		// List for storing temporary VMQ variables used for storing results of intermediate calculations.
    VAR_LIST temp_list_tail;		// Tail pointer for temporary variable list, convenient for insert.
    unsigned int temp_var_size;		// Size of temp vars, in bytes.
};

struct func_list_node
{
	int return_type;		// Return type of function, as indicated in cpp file (INT OR FLOAT).
	char* func_name;		// Name of function as found in cpp file.
	VAR_LIST var_list_head;		// Head of list for tracking func's local vars
	VAR_LIST var_list_tail;
	unsigned int var_size;		// Size of local (non-param) vars, in bytes.
	VAR_LIST param_list_head;	// List for tracking func's parameters
	VAR_LIST param_list_tail;
	struct VMQ_func_data VMQ_data;	// Tracks VMQ related data (VMQ statements, temporary variables, etc.)
	struct func_list_node* next;
};

typedef struct func_list_node* FUNC_LIST;

/* List Append Functions */
struct int_list_node* appendToIntList(char* intval);
struct flt_list_node* appendToFltList(char* fltval);
struct str_list_node* appendToStrList(char* strval);
struct var_list_node* appendToVarList(int var_type, char* var_name);
struct var_list_node* appendToParamList(int var_type, char* var_name);
struct VMQ_list_node* appendToVMQList(char* VMQ_line);
struct func_list_node* appendToFuncList(int return_type, char* func_name);
struct VMQ_mem_node*  appendToVMQMemList(int nodetype, void* node);

#endif
