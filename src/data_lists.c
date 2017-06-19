#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "data_lists.h"

/* List Append Functions */
struct int_list_node* appendToIntList(INT_LIST* list, char* val)
{
    if(!list) { yyerror("\n\tappendToIntList() - list ptr given NULL addr\n"); exit(-1); }

    struct int_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct int_list_node));
        (*list)->pil = newIntLit(val);
        (*list)->next = NULL;
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

struct flt_list_node* appendToFltList(FLT_LIST* list, char* val)
{
    if(!list) { yyerror("\n\tappendToFltList() - list ptr given NULL addr\n"); exit(-1); }
	
    struct flt_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct flt_list_node));
        (*list)->pfl = newFltLit(val);
        (*list)->next = NULL;
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

struct str_list_node* appendToStrList(STR_LIST* list, char* val)
{
    if(!list) { yyerror("\n\tappendToStrList() - list ptr given NULL addr\n"); exit(-1); }

    struct str_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct str_list_node));
        (*list)->psl = newStrLit(val);
        (*list)->next = NULL;
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

struct var_list_node* appendToVarList(VAR_LIST* list, int var_type, char* var_name, int isGlobal, int isParam)
{
    if(!list) { yyerror("\n\tappendToVarList() - list ptr given NULL addr\n"); exit(-1); }

    struct var_list_node* temp = *list;

    if(!(*list))
    {
	(*list) = malloc(sizeof(struct var_list_node));
	struct var* pv = newVar(var_type, var_name, isGlobal, isParam, 0);
	(*list)->pvr = newVarRef(&pv);
	(*list)->next = NULL;
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
	struct var* pv = newVar(var_type, var_name, isGlobal, isParam, 0);
	temp->pvr = newVarRef(&pv);
	temp->next = NULL;
    }

    return temp;
}

struct func_list_node* appendToFuncList(FUNC_LIST* list, char* func_name)
{
    if(!list) { yyerror("\n\tappendToFuncList() - list ptr given NULL addr\n"); exit(-1); }
	
    struct func_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct func_list_node));
        (*list)->func_name = strdup(func_name);
	(*list)->VMQ_var_list = NULL;
	(*list)->next = NULL;
    }
    else
    {
        while(temp->next)
	{
	    if(strcmp(func_name, temp->func_name) == 0)
		return temp;
	    else
		temp = temp->next;
	}
        temp = temp->next = malloc(sizeof(struct func_list_node));
        temp->func_name = strdup(func_name);
	temp->VMQ_var_list = NULL;
        temp->next = NULL;
    }

    return temp;
}
