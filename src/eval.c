#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_lists.h"
#include "error_handling.h"
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

			    // Put in placeholder line for stack frame creation op ("# <some non-negative integer>")
			    // We don't know the number needed for this op yet, as we don't yet know how many temp variables
			    // the function will need for intermediate calculations, which will increase the amount of required
			    // local memory.
			    appendToVMQList("");

			    eval(a->l); eval(a->r);

			    sprintf(CURRENT_FUNC->VMQ_data.stmt_list_head->VMQ_line, "# %d", CURRENT_FUNC->var_total_size + CURRENT_FUNC->VMQ_data.tempvar_max_size); 

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

    // Supporting data structures.
    char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
    struct var* v = NULL;
    struct varref* vnode = NULL;
    unsigned int result_type = 0;

    switch(a->nodetype)
    {
	case OUTPUT:	   
	case STREAMOUT:	    evalOutputStatement(a->l); evalOutputStatement(a->r); break;
	
	// Terminal cases
	case STR_LITERAL:   
	case ENDL:	    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "p #%d", ((struct str_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -11");
			    appendToVMQList("^ 2");
			    break;

	case INT_LITERAL:   VMQ_line = malloc(32);
			    sprintf(VMQ_line, "p #%d", ((struct int_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -9");
			    appendToVMQList("^ 2");
			    break;

	case FLT_LITERAL:   VMQ_line = malloc(32);
			    sprintf(VMQ_line, "p #%d", ((struct flt_node*)a)->val->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    break;
	
	// Expression cases:
	// ASSIGNOP, ADD_ASSIGN, SUB_ASSIGN, ADD, SUB, MUL, DIV, MOD, FUNC_CALL, VAR_ACCESS, ARR_ACCESS

	case ASSIGNOP:	    
	case ADD_ASSIGN:
	case SUB_ASSIGN:    if(a->nodetype == ASSIGNOP) evalAssignOpStatement(a);
			    else			evalIncOpStatement(a);
	case VAR_ACCESS:    
			    vnode = ((struct varref*)a->l);
			    v = vnode->val;
			    VMQ_line = malloc(32);
			    if(v->isGlobal)	VMQ_addr_prefix = "#";
			    else if(v->isParam)	VMQ_addr_prefix = "/";
			    else		VMQ_addr_prefix = "#/-";
			    sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    if(v->var_type == INT)  appendToVMQList("c 0 -9");
			    else		    appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    break;
			    
	case ADD:	    // The result of any calculation not being assigned to
	case SUB:	    // a variable is stored in the first temporary variable. 
	case MUL:	    
	case DIV:	    
	case MOD:	    if(a->nodetype == ADD || a->nodetype == SUB)    result_type = evalAddOpStatement(a);
			    else					    result_type = evalMulOpStatement(a);

			    if(result_type == INT)
			    {
				VMQ_line = malloc(32);
				sprintf(VMQ_line, "p #/-%d", CURRENT_FUNC->VMQ_data.tempvar_start);
				appendToVMQList(VMQ_line);
				free(VMQ_line);
				appendToVMQList("c 0 -9");
				appendToVMQList("^ 2");
			    }
			    else    // result of calculation is stored as a float.
			    {
				VMQ_line = malloc(32);
				if(CURRENT_FUNC->VMQ_data.tempvar_start % 4 == 0)
				    sprintf(VMQ_line, "p #/-%d", CURRENT_FUNC->VMQ_data.tempvar_start);
				else
				    sprintf(VMQ_line, "p #/-%d", CURRENT_FUNC->VMQ_data.tempvar_start + 2);
				appendToVMQList(VMQ_line);
				free(VMQ_line);
				appendToVMQList("c 0 -10");
				appendToVMQList("^ 2");
			    }
			    break;

	case FUNC_CALL:	    
			    break;

	case ARR_ACCESS:    
			    break;

	default:	    yyerror("evalOutputStatement() - Unknown nodetype encountered"); exit(-1);
    }
}

void evalAssignOpStatement(struct AST_node* a)
{
    if(!a || a->nodetype == 0)	return;
    if(!a->r || a->r->nodetype == 0) return;

    // Supporting data structures.
    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    char op_code;
    struct varref* l_node = ((struct var_node*)a->l)->val;
    struct var* l_val = l_node->val;
    struct varref* r_node = NULL;
    struct var* r_val = NULL;
    struct intlit* r_int = NULL;
    struct fltlit* r_flt = NULL;
    unsigned int result_type = 0;

    int ntype = a->r->nodetype;

    // Left child of (a) is guaranteed to be a variable.
    // Right child is either an ASSIGNOP, INCOP, ADDOP, MULOP, INT/FLT_LITERAL, VAR_ACCESS, FUNC_CALL, ARR_ACCESS
    switch(ntype)
    {
	case ASSIGNOP:	    evalAssignOpStatement(a->r);
			    r_node = ((struct var_node*)a->r->l)->val;
			    r_val = r_node->val;
			    
			    if(l_val->var_type == INT && r_val->var_type == INT)	    op_code = 'i';
			    else if(l_val->var_type == FLOAT && r_val->var_type == FLOAT)   op_code = 'I';
			    else if (l_val->var_type == INT && r_val->var_type == FLOAT)    op_code = 'f';
			    else							    op_code = 'F';
			    
			    if(l_val->isGlobal)	    VMQ_addr_l_prefix = "";
			    else if(l_val->isParam) VMQ_addr_l_prefix = "@/";
			    else		    VMQ_addr_l_prefix = "/-";
			    
			    if(r_val->isGlobal)	    VMQ_addr_r_prefix = "";
			    else if(r_val->isParam) VMQ_addr_r_prefix = "@/";
			    else		    VMQ_addr_r_prefix = "/-";
			    
			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c %s%d %s%d", op_code, VMQ_addr_r_prefix, r_node->VMQ_loc, VMQ_addr_l_prefix, l_node->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;

	case INT_LITERAL:   
	case FLT_LITERAL:   if(ntype == INT_LITERAL)
			    {
				r_int = ((struct int_node*)a->r)->val;
				if(l_val->var_type == INT)	op_code = 'i';
				else				op_code = 'F';
			    }
			    else
			    {
				r_flt = ((struct flt_node*)a->r)->val;
				if(l_val->var_type == INT)	op_code = 'f';
				else				op_code = 'I';
			    }

			    if(l_val->isGlobal)	    VMQ_addr_l_prefix = "";
			    else if(l_val->isParam) VMQ_addr_l_prefix = "@/";
			    else		    VMQ_addr_l_prefix = "/-";

			    VMQ_line = malloc(32);
			    if(r_int)	sprintf(VMQ_line, "%c %d %s%d", op_code, r_int->VMQ_loc, VMQ_addr_l_prefix, l_node->VMQ_loc);
			    else	sprintf(VMQ_line, "%c %d %s%d", op_code, r_int->VMQ_loc, VMQ_addr_l_prefix, l_node->VMQ_loc);

			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;
	
	case VAR_ACCESS:
	
			    break;

	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    break;
	
	case ARR_ACCESS:    break;
    }
}

void evalIncOpStatement(struct AST_node* a)
{

}

unsigned int evalAddOpStatement(struct AST_node* a)
{

}

unsigned int evalMulOpStatement(struct AST_node* a)
{

}
