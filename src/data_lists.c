#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "helper_functions.h"
#include "data_lists.h"
#include "parser.tab.h"
/* List Append Functions */
struct int_list_node* appendToIntList(char* val)
{
    struct int_list_node* temp = INT_LIST_TAIL;

    if(!temp)
    {
        temp = INT_LIST_HEAD = INT_LIST_TAIL = malloc(sizeof(struct int_list_node));
        temp->pil = newIntLit(val);
	temp->next = NULL;
    }
    else
    {
/*
 *	O(n) scan through existing list elements to prevent duplicates.
 *
 *	Ideally, we could improve to ~O(1) by using a hash table.  It'd be a bit
 *	of a memory hog to do this for each data list, but could improve performance.
 *
 *	Alternatively, we could compromise and improve to O(log n) by using a BST,
 *	since all values must be unique anyway.  It'd be a good idea to implement
 *	a self-balancing BST to ensure that the BST lookup function doesn't degrade
 *	into an O(n) algorithm.
 *
 *	Realistically however, for our subset of C++ it would be unlikely that each 
 *	function would contain more than a reasonable number of variables to justify
 *	the existing O(n) algorithm.
 *
 *	Something else for the to-do list, perhaps?
*/
	struct int_list_node* scanner = INT_LIST_HEAD;
	while(scanner)
	{
	    if(strcmp(scanner->pil->val, val) == 0)
		return scanner;

	    scanner = scanner->next;
	}

	temp = INT_LIST_TAIL = INT_LIST_TAIL->next = malloc(sizeof(struct int_list_node));
        temp->pil= newIntLit(val);
        temp->next = NULL;
    }

	return temp;
}

struct flt_list_node* appendToFltList(char* val)
{
    struct flt_list_node* temp = FLT_LIST_TAIL;

    if(!temp)
    {
        temp = FLT_LIST_HEAD = FLT_LIST_TAIL = malloc(sizeof(struct flt_list_node));
        temp->pfl = newFltLit(val);
        temp->next = NULL;
    }
    else
    {
	struct flt_list_node* scanner = FLT_LIST_HEAD;
	while(scanner)
	{
	    if(strcmp(scanner->pfl->val, val) == 0)
		return scanner;

	    scanner = scanner->next;
	}

        temp = FLT_LIST_TAIL = FLT_LIST_TAIL->next = malloc(sizeof(struct flt_list_node));
        temp->pfl = newFltLit(val);
        temp->next = NULL;
    }

    return temp;
}

struct str_list_node* appendToStrList(char* val)
{
    struct str_list_node* temp = STR_LIST_TAIL;

    if(!temp)
    {
        temp = STR_LIST_HEAD = STR_LIST_TAIL = malloc(sizeof(struct str_list_node));
        temp->psl = newStrLit(val);
        temp->next = NULL;
    }
    else
    {
	struct str_list_node* scanner = STR_LIST_HEAD;
	while(scanner)
	{
	    if(strcmp(scanner->psl->val, val) == 0)
		return scanner;

	    scanner = scanner->next;
	}

        temp = STR_LIST_TAIL = STR_LIST_TAIL->next = malloc(sizeof(struct str_list_node));
        temp->psl = newStrLit(val);
	temp->next = NULL;
    }

    return temp;
}

struct var_list_node* appendToVarList(int var_type, char* var_name)
{
    struct var_list_node* temp;
    int isGlobal = 0;
    int isParam = 0;
    int size = 1;   // size of variable (where size > 1 is an array) is set by parser if var is an array,
		    // as determining size requires more data that is not immediately available to lexer.
		    // (The lexer is the callee of this function).
    
    if(SCOPE_STACK_HEAD == GLOBAL_SCOPE)
    {
	isGlobal = 1;
	temp = GLOBAL_VAR_LIST_TAIL;
    }
    else
    {
	if(SCOPE_STACK_HEAD->isNewScope)
	{
	    isParam = 1;
	    temp = CURRENT_FUNC->param_list_tail;
	}
	else
	    temp = CURRENT_FUNC->var_list_tail;
    }

    if(!temp)
    {
	if(isGlobal)
	    temp = GLOBAL_VAR_LIST_HEAD = GLOBAL_VAR_LIST_TAIL = malloc(sizeof(struct var_list_node));
	else if(isParam)
	    temp = CURRENT_FUNC->param_list_head = CURRENT_FUNC->param_list_tail = malloc(sizeof(struct var_list_node));
	else
	    temp = CURRENT_FUNC->var_list_head = CURRENT_FUNC->var_list_tail = malloc(sizeof(struct var_list_node));

	struct var* pv = newVar(var_type, var_name, isGlobal, isParam, size);
	temp->pvr = newVarRef(&pv);
	temp->next = NULL;
    }
    else
    {
	temp = temp->next = malloc(sizeof(struct var_list_node));
	if(isGlobal)
	    GLOBAL_VAR_LIST_TAIL = GLOBAL_VAR_LIST_TAIL->next = temp;
	else if(isParam)
	    CURRENT_FUNC->param_list_tail = CURRENT_FUNC->param_list_tail->next = temp;
	else
	    CURRENT_FUNC->var_list_tail = CURRENT_FUNC->var_list_tail->next = temp;

	struct var* pv = newVar(var_type, var_name, isGlobal, isParam, size);
	temp->pvr = newVarRef(&pv);
	temp->next = NULL;
    }

    return temp;
}

struct VMQ_list_node* appendToVMQList(char* VMQ_line)
{
    if(!CURRENT_FUNC)
	yyerror("appendToVMQList() - CURRENT_FUNC is NULL, cannot append VMQ statement");

    struct VMQ_list_node* temp = CURRENT_FUNC->VMQ_data.stmt_list_tail;
    if(!temp)
	temp = CURRENT_FUNC->VMQ_data.stmt_list_head = CURRENT_FUNC->VMQ_data.stmt_list_tail = malloc(sizeof(struct VMQ_list_node));
    else
	temp = CURRENT_FUNC->VMQ_data.stmt_list_tail = CURRENT_FUNC->VMQ_data.stmt_list_tail->next = malloc(sizeof(struct VMQ_list_node));
	
    temp->VMQ_line = strdup(VMQ_line);
    temp->next = NULL;

    return temp;
}

struct func_list_node* appendToFuncList(int return_type, char* func_name)
{
    struct func_list_node* temp = CURRENT_FUNC;
    if(!temp)
	temp = FUNC_LIST_HEAD = CURRENT_FUNC = malloc(sizeof(struct func_list_node));
    else
	temp = CURRENT_FUNC = CURRENT_FUNC->next = malloc(sizeof(struct func_list_node));

    struct VMQ_func_data* tempVMQ = &(temp->VMQ_data);

    temp->return_type = return_type;
    temp->func_name = strdup(func_name);
    temp->var_list_head = temp->var_list_tail = NULL;
    temp->var_size = 0;
    temp->param_list_head = temp->param_list_tail = NULL;
    
    tempVMQ->stmt_list_head = tempVMQ->stmt_list_tail = NULL;
    tempVMQ->stmt_count = 0;
    tempVMQ->quad_start_line = tempVMQ->quad_end_line = 1;
    tempVMQ->temp_list_head = tempVMQ->temp_list_tail = NULL;
    tempVMQ->temp_var_size = 0;

    temp->next = NULL;

    return temp;
}

struct VMQ_mem_node* appendToVMQMemList(int nodetype, void* node)
{
    if(!(nodetype != INT_LITERAL || nodetype != FLT_LITERAL || nodetype != STR_LITERAL
	 ||  nodetype != INT || nodetype != FLOAT))
	yyerror("appendToVMQMemList() - Invalid nodetype argument");

    struct VMQ_mem_node* temp = VMQ_MEM_LIST_TAIL;
    if(!temp)
	temp = VMQ_MEM_LIST_HEAD = VMQ_MEM_LIST_TAIL = malloc(sizeof(struct VMQ_mem_node));
    else
	temp = VMQ_MEM_LIST_TAIL = VMQ_MEM_LIST_TAIL->next = malloc(sizeof(struct VMQ_mem_node));

    temp->nodetype = nodetype;
    temp->node = node;

    return temp;
}
