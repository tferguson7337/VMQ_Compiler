#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "data_lists.h"

/* List Append Functions */
struct int_list_node* appendToIntList(INT_LIST* list, struct intlit** val)
{
	if(!list) { yyerror("\n\tappendToIntList() - list ptr given NULL addr\n"); exit(-1); }

	struct int_list_node* temp = *list;

	if(!(*list))
    {
        (*list) = malloc(sizeof(struct int_list_node));
        (*list)->pil = *val;
        (*list)->next = NULL;
	}
    else
    {
        while(temp->next) temp = temp->next;

        temp = temp->next = malloc(sizeof(struct int_list_node));
        temp->pil= *val;
        temp->next = NULL;
	}

	return temp;
}

struct flt_list_node* appendToFltList(FLT_LIST* list, struct fltlit** val)
{
    if(!list) { yyerror("\n\tappendToFltList() - list ptr given NULL addr\n"); exit(-1); }
	
    struct flt_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct flt_list_node));
        (*list)->pfl = *val;
        (*list)->next = NULL;
    }
    else
    {
        while(temp->next) temp = temp->next;

        temp = temp->next = malloc(sizeof(struct flt_list_node));
        temp->pfl = *val;
        temp->next = NULL;
    }

    return temp;
}

struct str_list_node* appendToStrList(STR_LIST* list, struct strlit** val)
{
    if(!list) { yyerror("\n\tappendToStrList() - list ptr given NULL addr\n"); exit(-1); }

    struct str_list_node* temp = *list;

    if(!(*list))
    {
        (*list) = malloc(sizeof(struct str_list_node));
        (*list)->psl = *val;
        (*list)->next = NULL;
    }
    else
    {
        while(temp->next) temp = temp->next;

        temp = temp->next = malloc(sizeof(struct str_list_node));
        temp->psl = *val;
	temp->next = NULL;
    }

    return temp;
}

struct var_list_node* appendToVarList(VAR_LIST* list, struct varref** val)
{
	if(!list) { yyerror("\n\tappendToVarList() - list ptr given NULL addr\n"); exit(-1); }

	struct var_list_node* temp = *list;

	if(!(*list))
	{
		(*list) = malloc(sizeof(struct var_list_node));
		(*list)->pvr = *val;
		(*list)->next = NULL;
	}
	else
	{
		while(temp->next) temp = temp->next;

		temp = temp->next = malloc(sizeof(struct var_list_node));
		temp->pvr = *val;
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
        while(temp->next) temp = temp->next;

        temp = temp->next = malloc(sizeof(struct func_list_node));
        temp->func_name = strdup(func_name);
		temp->VMQ_var_list = NULL;
        temp->next = NULL;
	}

	return temp;
}
