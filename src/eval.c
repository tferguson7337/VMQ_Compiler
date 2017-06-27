#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_lists.h"
#include "eval.h"
#include "helper_functions.h"
#include "parser.tab.h"

void eval(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    switch(a->nodetype)
    {
	// Higher level statement cases.
	case RETURN:	    evalReturnStatement(a);	break;
	case IF:	    evalIfStatement(a);		break;
	case WHILE:	    evalWhileStatement(a);	break;
	case INPUT:	    evalInputStatement(a);	break;
	case OUTPUT:	    evalOutputStatement(a);	break;

	// ASSIGNOP
	case ASSIGNOP:	    evalAssignOpStatement(a);	break;
	
	// INCOPs
	case ADD_ASSIGN:
	case SUB_ASSIGN:    evalIncOpStatement(a);	break;
	
	// ADDOPs
	case ADD:
	case SUB:	    evalAddOpStatement(a);	break;

	// MULOPs
	case MUL:
	case DIV:
	case MOD:	    evalMulOpStatement(a);	break;

	// Terminal cases are not handled here; return when encountered.
	case INT_LITERAL:
	case FLT_LITERAL:
	case STR_LITERAL:
	case ID:
	case ENDL:
	case VAR_DEC:
	case ARR_DEC:
	case VAR_ACCESS:
	case ARR_ACCESS:    return;

	// CURRENT_FUNC pointer should change when encountering a function header (part of FUNC_DEF).
	case FUNC_DEF:	    if(!CURRENT_FUNC)
				CURRENT_FUNC = FUNC_LIST_HEAD;
			    else
				CURRENT_FUNC = CURRENT_FUNC->next;
			    eval(a->l); eval(a->r);

			    if(strcmp(CURRENT_FUNC->func_name, "main") == 0)
				appendToVMQList("h");
			    else
				appendToVMQList("/");

			    break;

	case PROG:	    CURRENT_FUNC = NULL;
			    eval(a->l);	eval(a->r);
			    break;

	// All other non-terminal cases are traversed.
	default:	    eval(a->l); eval(a->r);
    }
}

void evalReturnStatement(struct AST_node* a)
{

}

void evalIfStatement(struct AST_node* a)
{

}

void evalWhileStatement(struct AST_node* a)
{

}

void evalInputStatement(struct AST_node* a)
{

}

void evalOutputStatement(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    switch(a->nodetype)
    {
	char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
	struct var* v = NULL;
	struct varref* vnode = NULL;

	case OUTPUT:	   
	case STREAMOUT:	    evalOutputStatement(a->l); evalOutputStatement(a->r); break;
	
	// Terminal cases
	case STR_LITERAL:   
	case ENDL:	    VMQ_line = malloc(26);
			    sprintf(VMQ_line, "p #%d", ((struct str_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -11");
			    appendToVMQList("^ 2");
			    break;

	case INT_LITERAL:   VMQ_line = malloc(26);
			    sprintf(VMQ_line, "p #%d", ((struct int_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -9");
			    appendToVMQList("^ 2");
			    break;

	case FLT_LITERAL:   VMQ_line = malloc(26);
			    sprintf(VMQ_line, "p #%d", ((struct flt_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    break;
	
	// Expression cases:
	// ASSIGNOP, ADD_ASSIGN, SUB_ASSIGN, ADD, SUB, MUL, DIV, MOD, FUNC_CALL, VAR_ACCESS, ARR_ACCESS, INT_LITERAL, FLT_LITERAL

	case ASSIGNOP:	    
	case ADD_ASSIGN:
	case SUB_ASSIGN:    if(a->nodetype == ASSIGNOP) evalAssignOpStatement(a);
			    else			evalIncOpStatement(a);
			    vnode = ((struct varref*)a->l);
			    v = vnode->val;
			    VMQ_line = malloc(26);
			    VMQ_addr_prefix = malloc(26);
			    if(v->isGlobal)	VMQ_addr_prefix = "#";
			    else if(v->isParam)	VMQ_addr_prefix = "/";
			    else		VMQ_addr_prefix = "#/-";
			    sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    free(VMQ_addr_prefix);
			    if(v->var_type == INT)  appendToVMQList("c 0 -9");
			    else		    appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    break;
			    
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    if(a->nodetype == ADD || a->nodetype == SUB)    evalAddOpStatement(a);
			    else					    evalMulOpStatement(a);
			    
			    
    }
}

void evalAssignOpStatement(struct AST_node* a)
{

}

void evalIncOpStatement(struct AST_node* a)
{

}

void evalAddOpStatement(struct AST_node* a)
{

}

void evalMulOpStatement(struct AST_node* a)
{

}
