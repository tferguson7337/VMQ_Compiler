#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "helper_functions.h"
#include "data_lists.h"

/* List Append Functions */
struct int_list_node* appendToIntList(char* val)
{
    struct int_list_node* temp = INT_LIST_HEAD;

    if(!temp)
    {
        temp = INT_LIST_HEAD  = malloc(sizeof(struct int_list_node));
        temp->pil = newIntLit(val);
	temp->next = NULL;
    }
    else
    {
        while(temp->next)
	{
	    if(strcmp(val, temp->pil->val) == 0)
		return temp;
	    else
		temp = temp->next;
	}
        temp = temp->next = malloc(sizeof(struct int_list_node));
        temp->pil= newIntLit(val);
        temp->next = NULL;
    }

	return temp;
}

struct flt_list_node* appendToFltList(char* val)
{
    struct flt_list_node* temp = FLT_LIST_HEAD;

    if(!temp)
    {
        temp = FLT_LIST_HEAD = malloc(sizeof(struct flt_list_node));
        temp->pfl = newFltLit(val);
        temp->next = NULL;
    }
    else
    {
        while(temp->next)
	{
	    if(strcmp(val, temp->pfl->val) == 0)
		return temp;
	    else
		temp = temp->next;
	}

        temp = temp->next = malloc(sizeof(struct flt_list_node));
        temp->pfl = newFltLit(val);
        temp->next = NULL;
    }

    return temp;
}

struct str_list_node* appendToStrList(char* val)
{
    struct str_list_node* temp = STR_LIST_HEAD;

    if(!temp)
    {
        temp = STR_LIST_HEAD = malloc(sizeof(struct str_list_node));
        temp->psl = newStrLit(val);
        temp->next = NULL;
    }
    else
    {
        while(temp->next)
	{
	    if(strcmp(val, temp->psl->val) == 0)
		return temp;
	    else
		temp = temp->next;
	}
        temp = temp->next = malloc(sizeof(struct str_list_node));
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
    int size = 0;   // size of variable (where size > 1 is an array) is set by parser,
		    // as determining size requires more data that is not immediately available to lexer.
    
    if(SCOPE_STACK_HEAD == GLOBAL_SCOPE)
    {
	temp = GLOBAL_VAR_LIST_HEAD;
	isGlobal = 1;
    }
    else
    {
	temp = CURRENT_FUNC->var_list;
	if(SCOPE_STACK_HEAD->isNewScope)
	    isParam = 1;
    }

    if(!temp)
    {
	if(isGlobal)
	    temp = GLOBAL_VAR_LIST_HEAD = malloc(sizeof(struct var_list_node));
	else
	    temp = CURRENT_FUNC->var_list = malloc(sizeof(struct var_list_node));

	struct var* pv = newVar(var_type, var_name, isGlobal, isParam, size);
	temp->pvr = newVarRef(&pv);
	temp->next = NULL;
    }
    else
    {
	while(temp->next)
	{
	    if(strcmp(var_name, temp->pvr->val->var_name) == 0)
		return temp;
	    else
		temp = temp->next;
	}
	temp = temp->next = malloc(sizeof(struct var_list_node));
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

    struct VMQ_list_node* temp = CURRENT_FUNC->VMQ_list_tail;
    if(!temp)
    {
	temp = CURRENT_FUNC->VMQ_list_head = CURRENT_FUNC->VMQ_list_tail = malloc(sizeof(struct VMQ_list_node));
	temp->VMQ_line = strdup(VMQ_line);
	temp->next = NULL;
    }
    else
    {
	temp = temp->next = malloc(sizeof(struct VMQ_list_node));
	temp->VMQ_line = strdup(VMQ_line);
	temp->next = NULL;
    }

    return temp;
}

struct func_list_node* appendToFuncList(int return_type, char* func_name)
{
    struct func_list_node* temp;
    if(!FUNC_LIST_HEAD)
    {
	temp = FUNC_LIST_HEAD = CURRENT_FUNC = malloc(sizeof(struct func_list_node));

	temp->return_type = return_type;
	temp->func_name = strdup(func_name);
	temp->var_list = NULL;
	temp->var_count = 0;
	temp->param_count = 0;
	temp->VMQ_start_line = 0;
	temp->VMQ_end_line = 0;
	temp->next = NULL;
    }
    else
    {
	temp = CURRENT_FUNC = CURRENT_FUNC->next = malloc(sizeof(struct func_list_node));

	temp->return_type = return_type;
	temp->func_name = strdup(func_name);
	temp->var_list = NULL;
	temp->var_count = 0;
	temp->param_count = 0;
	temp->VMQ_start_line = 0;
	temp->VMQ_end_line = 0;
	temp->next = NULL;
    }

    return temp;
}
