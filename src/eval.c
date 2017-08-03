#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_lists.h"
#include "eval.h"

void eval(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    struct func_list_node* func = NULL;

    switch(a->nodetype)
    {
	// Higher level statement cases.
	case RETURN:	    evalReturn(a);	break;
	case IF:	    evalIf(a);		break;
	case WHILE:	    evalWhile(a);	break;
	case INPUT:	    evalInput(a);	break;
	case OUTPUT:	    evalOutput(a);	break;

	// ASSIGNOP
	case ASSIGNOP:	    evalAssignOp(a);	break;
	
	// INCOPs
	case ADD_ASSIGN:
	case SUB_ASSIGN:    evalIncOp(a);	break;
	
	// ADDOPs
	case ADD:
	case SUB:	    evalAddOp(a);	break;

	// MULOPs
	case MUL:
	case DIV:
	case MOD:	    evalMulOp(a);	break;
	
	// FUNC_CALL - function call outside of assigment, throw away the return value (operand 1 == 0)
	case FUNC_CALL:	    func = ((struct func_node*)a->l)->val;
			    evalFuncCall(a, func->return_type);
			    char* VMQ_line = malloc(32);
			    sprintf(VMQ_line, "c 0 %d", func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    break;

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
			    {
				CURRENT_FUNC = FUNC_LIST_HEAD;
				CURRENT_FUNC->VMQ_data.quad_start_line = 1;
				CURRENT_FUNC->VMQ_data.quad_end_line = 0;   // All functions are guaranteed to have 2 ops
			    }
			    else
			    {
				struct func_list_node* prev = CURRENT_FUNC;
				CURRENT_FUNC = CURRENT_FUNC->next;
				CURRENT_FUNC->VMQ_data.quad_start_line = prev->VMQ_data.quad_end_line + 1;
				CURRENT_FUNC->VMQ_data.quad_end_line = CURRENT_FUNC->VMQ_data.quad_start_line - 1;
			    }
			    // Put in placeholder line for stack frame creation op ("# <some non-negative integer>")
			    // We don't know the number needed for this op yet, as we don't yet know how many temp variables
			    // the function will need for intermediate calculations, which will increase the amount of required
			    // local memory.
			    appendToVMQList("");

			    eval(a->r);
			    
			    // Modify the placeholder line created above.
			
			    // If the function has declared local variables, add two bytes for the padding.
			    if(CURRENT_FUNC->var_total_size)
				CURRENT_FUNC->var_total_size += 2;

			    // If the function ended up using any temporary variables, add two bytes for padding.
			    if(CURRENT_FUNC->VMQ_data.tempvar_max_size)
				CURRENT_FUNC->VMQ_data.tempvar_max_size += 2;
 
			    unsigned int total_size = CURRENT_FUNC->var_total_size + CURRENT_FUNC->VMQ_data.tempvar_max_size;

			    sprintf(CURRENT_FUNC->VMQ_data.stmt_list_head->VMQ_line, "# %d", total_size); 
			    
			    if(strcmp(CURRENT_FUNC->func_name, "main") == 0)
				appendToVMQList("h");	// Program ends after returning from main().
			    else
				appendToVMQList("/");	// Op for returning to caller function().

			    break;

	case PROG:	    CURRENT_FUNC = NULL;
			  //eval(a->l);  
			    eval(a->r);
			    break;

	// All other non-terminal cases are traversed.
	default:	    eval(a->l); eval(a->r);
    }
}

void evalReturn(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    // Supporting data structures.
    char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
    char op_code;
    struct var* v = NULL;
    struct varref* vnode = NULL;
    struct func_list_node* func = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    unsigned int temp_addr;

    switch(a->nodetype)
    {	
	// left child of RETURN node is any type of expression, right child is guaranteed to be NULL
	case RETURN:	    evalReturn(a->l); break;
	
	// Insert variable's value into correct return value memory space
	case ASSIGNOP:	
	case ADD_ASSIGN:    
	case SUB_ASSIGN:    if(a->nodetype == ASSIGNOP)
				evalAssignOp(a);
			    else
				evalIncOp(a);

			    VMQ_line = malloc(32);
			    
			    if(a->l->nodetype == VAR_ACCESS)
			    {
				vnode = ((struct var_node*)a->l)->val;
				v = vnode->val;

				if(v->var_type == INT)  op_code = 'i';
				else			op_code = 'I';

				if(v->isGlobal)	    VMQ_addr_prefix = "";
				else if(v->isParam) VMQ_addr_prefix = "@/";
				else		    VMQ_addr_prefix = "/-";

				sprintf(VMQ_line, "%c %s%d @/4", op_code, VMQ_addr_prefix, vnode->VMQ_loc);
				appendToVMQList(VMQ_line);
			    }
			    else // a->l->nodetype == ARR_ACCESS
			    {
				vnode = ((struct var_node*)a->l->l)->val;
				v = vnode->val;

				if(v->var_type == INT)  op_code = 'i';
				else			op_code = 'I';

				sprintf(VMQ_line, "%c @/-%d @/4", op_code, CURRENT_FUNC->VMQ_data.tempvar_start);
				appendToVMQList(VMQ_line);
			    }
			    
			    free(VMQ_line);

			    break;

	// Insert result of math op into correct return value memory space.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    if(a->nodetype == ADD || a->nodetype == SUB)
				evalAddOp(a);
			    else
				evalMulOp(a);

			    if(result->type == INT)
				op_code = 'i';
			    else // result->type == FLOAT
				op_code = 'I';

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c /-%d @/4", op_code, result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
	
			    break;

	// Insert variable's value into correct return value memory space.
	case VAR_ACCESS:    vnode = ((struct var_node*)a)->val;
			    v = vnode->val;

			    if(v->var_type == INT)  op_code = 'i';
			    else		    op_code = 'I';

			    if(v->isGlobal)	VMQ_addr_prefix = "";
			    else if(v->isParam)	VMQ_addr_prefix = "@/";
			    else		VMQ_addr_prefix = "/-";
			    
			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c %s%d @/4", op_code, VMQ_addr_prefix, vnode->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    
			    break;
	
	// Insert return value of function call into return value memory space of current function.
	case FUNC_CALL:	    func = ((struct func_node*)a->l)->val;
			    evalFuncCall(a, func->return_type);

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "c @/4 %d", func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;

	// Insert address of literal integer or float into return value memory space.
	case INT_LITERAL:
	case FLT_LITERAL:   VMQ_line = malloc(32);
			    if(a->nodetype == INT_LITERAL)
				sprintf(VMQ_line, "i %d @/4", ((struct int_node*)a)->val->VMQ_loc);
			    else
				sprintf(VMQ_line, "I %d @/4", ((struct flt_node*)a)->val->VMQ_loc);

			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;

	// Insert address of array element into return value memory space.
	case ARR_ACCESS:    evalArrAccess(a);

			    vnode = ((struct var_node*)a->l)->val;
			    v = vnode->val;

			    if(v->var_type == INT)  op_code = 'i';
			    else		    op_code = 'I';

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c @/-%d @/4", op_code, result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;
    }
}

void evalIf(struct AST_node* a)
{

}

void evalWhile(struct AST_node* a)
{

}

void evalInput(struct AST_node* a)
{
    if(!a || a->nodetype == 0)	return;

    // Supporting data structures.
    char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
    struct var* v = NULL;
    struct varref* vnode = NULL;
    unsigned int result_type = 0;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;

    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

    switch(a->nodetype)
    {
	// Traverse the subtree.
	case INPUT:	evalInput(a->l); break;

	// Get input for the variable located at the node to the right.
	case STREAMIN:	evalInput(a->l);
	
			VMQ_line = malloc(32);
			if(a->r->nodetype == VAR_ACCESS)
			{
			    vnode = ((struct var_node*)a->r)->val;
			    v = vnode->val;
			    if(v->isGlobal)	    VMQ_addr_prefix = "#";
			    else if(v->isParam)	    VMQ_addr_prefix = "/";
			    else		    VMQ_addr_prefix = "#/-";

			    sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
			    appendToVMQList(VMQ_line);
			}
			else // a->r->nodetype == ARR_ACCESS
			{
			    evalArrAccess(a->r);
			    vnode = ((struct var_node*)a->r->l)->val;
			    v = vnode->val;
			    
			    sprintf(VMQ_line, "p /-%d", result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			}

			free(VMQ_line);
			
			// Call correct input function (-1 for INT, -2 for FLOAT)
			if(v->var_type == INT)
			    appendToVMQList("c 0 -1");
			else
			    appendToVMQList("c 0 -2");
			break;			    
    }

    while(CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
	freeTempVar();
}

void evalOutput(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    // Supporting data structures.
    char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
    struct var* v = NULL;
    struct varref* vnode = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    struct func_list_node* func = NULL;
    unsigned int temp_addr;

    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

    switch(a->nodetype)
    {
	// High level cases - Traverse the subtrees.
	case OUTPUT:	   
	case STREAMOUT:	    evalOutput(a->l); evalOutput(a->r); break;
	
	// Terminal cases - directly output string, integer, or float value.
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

	// Output value of variable, possibly used in ASSIGNOP/INCOP expression
	case ASSIGNOP:	    
	case ADD_ASSIGN:
	case SUB_ASSIGN:    if(a->nodetype == ASSIGNOP)
				evalAssignOp(a);
			    else
				evalIncOp(a);
	case VAR_ACCESS:
	case ARR_ACCESS:    if(a->nodetype == VAR_ACCESS)
			    {
				vnode = ((struct var_node*)a)->val;
				v = vnode->val;
			    }
			    else if(a->nodetype == ARR_ACCESS)
			    {
				vnode = ((struct var_node*)a->l)->val;
				v = vnode->val;

				evalArrAccess(a);
			    }
			    else // a->nodetype == ASSIGNOP || ADD_ASSIGN || SUB_ASSIGN
			    {
				if(a->l->nodetype == VAR_ACCESS)
				{
				    vnode = ((struct var_node*)a->l)->val;
				    v = vnode->val;
				}
				else // a->l->nodetype == ARR_ACCESS
				{
				    vnode = ((struct var_node*)a->l->l)->val;
				    v = vnode->val;
				}
			    }
			    
			    VMQ_line = malloc(32);
			    
			    if(v->size == 1)
			    {
				if(v->isGlobal)	    VMQ_addr_prefix = "#";
				else if(v->isParam) VMQ_addr_prefix = "/";
				else		    VMQ_addr_prefix = "#/-";

				sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
			    }
			    else // v->size > 1 (i.e., var is an array element)
				sprintf(VMQ_line, "p /-%d", result->VMQ_loc);
				
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    if(v->var_type == INT)  appendToVMQList("c 0 -9");
			    else		    appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    break;
			    
	// Output result of math operation.
	case ADD:
	case SUB:
	case MUL:	    
	case DIV:	    
	case MOD:	    if(a->nodetype == ADD || a->nodetype == SUB)
				evalAddOp(a);
			    else
				evalMulOp(a);
	    
			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "p #/-%d", result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    
			    if(result->type == INT)
				appendToVMQList("c 0 -9");
			    else
				appendToVMQList("c 0 -10");

			    appendToVMQList("^ 2");

			    break;

	// Output return value of function call.
	case FUNC_CALL:	    func = ((struct func_node*)a->l)->val;
			    evalFuncCall(a, func->return_type);

			    // Store return value of function into a temporary variable.
			    if(func->return_type == INT)
				temp_addr = getNewTempVar(INT);
			    else
				temp_addr = getNewTempVar(FLOAT);

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "p #/-%d", temp_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    freeTempVar();
			    
			    if(func->return_type == INT)
				appendToVMQList("c 0 -9");
			    else
				appendToVMQList("c 0 -10");
			    appendToVMQList("^ 2");
			    
			    break;

	// No other nodetypes should be encountered - output error and quit if one is encountered.
	default:	    yyerror("evalOutput() - Unknown nodetype encountered"); exit(-1);
    }

    while(orig_size != CURRENT_FUNC->VMQ_data.tempvar_cur_size)
	freeTempVar();
}

void evalAssignOp(struct AST_node* a)
{
    if(!a || a->nodetype == 0)		return;
    if(!a->r || a->r->nodetype == 0)	return;

    // Supporting data structures, shortens code in various switch cases.
    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    char op_code;
    struct varref* l_node;
    struct var* l_val;
    struct varref* r_node = NULL;
    struct var* r_val = NULL;
    struct intlit* r_int = NULL;
    struct fltlit* r_flt = NULL;
    struct func_list_node* func = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;

    int ntype = a->r->nodetype;

    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
    unsigned int lhs_addr, rhs_addr;

    // Get the VMQ location values and pointers to the right data structures set up for the LHS of ASSIGN_OP
    if(a->l->nodetype == VAR_ACCESS)
    {
	l_node = ((struct var_node*)a->l)->val;
	l_val = l_node->val;
	lhs_addr = l_node->VMQ_loc;

	if(l_val->isGlobal)	VMQ_addr_l_prefix = "";
	else if(l_val->isParam) VMQ_addr_l_prefix = "@/";
	else			VMQ_addr_l_prefix = "/-";
			    
    }
    else // a->l->nodetype == ARR_ACCESS
    {
	l_node = ((struct var_node*)a->l->l)->val;
	l_val = l_node->val;
	// There are some cases where lhs_addr must be done *after* rhs_addr,
	// so it is handled in each case.

	VMQ_addr_l_prefix = "@/-";
    }

    // Left child of (a) is guaranteed to be a variable.
    // Right child is either an ASSIGNOP, INCOP, ADDOP, MULOP, INT/FLT_LITERAL, VAR_ACCESS, FUNC_CALL, ARR_ACCESS
    switch(ntype)
    {
	// Place value of right child into left child (variable).
	case ASSIGNOP:	    
	case ADD_ASSIGN:
	case SUB_ASSIGN:    if(ntype == ASSIGNOP)   
				evalAssignOp(a->r);
			    else
				evalIncOp(a->r);

			    if(a->r->l->nodetype == VAR_ACCESS)
			    {
				r_node = ((struct var_node*)a->r->l)->val;
				r_val = r_node->val;

				rhs_addr = r_node->VMQ_loc;

				if(r_val->isGlobal)	VMQ_addr_r_prefix = "";
				else if(r_val->isParam) VMQ_addr_r_prefix = "@/";
				else			VMQ_addr_r_prefix = "/-";
			    }
			    else // a->r->l->nodetype == ARR_ACCESS
			    {
				r_node = ((struct var_node*)a->r->l->l)->val;
				r_val = r_node->val;

				rhs_addr = getNewTempVar(ADDR);
				VMQ_addr_r_prefix = "@/-";
			    }
			    r_val = r_node->val;
			    
			    if(l_val->var_type == INT && r_val->var_type == INT)	    op_code = 'i';
			    else if(l_val->var_type == FLOAT && r_val->var_type == FLOAT)   op_code = 'I';
			    else if (l_val->var_type == INT && r_val->var_type == FLOAT)    op_code = 'f';
			    else							    op_code = 'F';			    

			    if(a->l->nodetype == ARR_ACCESS)
			    {
				evalArrAccess(a->l);
				lhs_addr = result->VMQ_loc;
			    }

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c %s%d %s%d", op_code, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    if(a->r->l->nodetype == ARR_ACCESS)
				freeTempVar();

			    break;
	
	// Place value of literal into variable.
	case INT_LITERAL:   
	case FLT_LITERAL:   if(ntype == INT_LITERAL)
			    {
				rhs_addr = ((struct int_node*)a->r)->val->VMQ_loc;
				if(l_val->var_type == INT)	op_code = 'i';
				else				op_code = 'F';
			    }
			    else
			    {
				rhs_addr = ((struct flt_node*)a->r)->val->VMQ_loc;
				if(l_val->var_type == INT)	op_code = 'f';
				else				op_code = 'I';
			    }

			    if(a->l->nodetype == ARR_ACCESS)
			    {
				evalArrAccess(a->l);
				lhs_addr = result->VMQ_loc;
			    }

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c %d %s%d", op_code, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;
	
	// Place value of r_val variable into l_val variable.
	case VAR_ACCESS:    r_node = ((struct var_node*)a->r)->val;
			    r_val = r_node->val;
			    rhs_addr = r_node->VMQ_loc;
			    if(l_val->var_type == r_val->var_type)
			    {
				if(l_val->var_type == INT_LITERAL)  op_code = 'i';
				else				    op_code = 'I';
			    }
			    else
			    {
				if(l_val->var_type == INT_LITERAL)  op_code = 'f';
				else				    op_code = 'F';
			    }
			    
			    if(r_val->isGlobal)	    VMQ_addr_r_prefix = "";
			    else if(r_val->isParam) VMQ_addr_r_prefix = "@/";
			    else		    VMQ_addr_r_prefix = "/-";

			    if(a->l->nodetype == ARR_ACCESS)
			    {
				evalArrAccess(a->l);
				lhs_addr = result->VMQ_loc;
			    }

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c %s%d %s%d", op_code, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
	
			    break;

	// Place result of math op into l_val variable.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    if(a->l->nodetype == ARR_ACCESS)
			    {
				evalArrAccess(a->l);

				// We call getNewTempVar here to keep the result of the math op
				// below from overwritting the address stored from the ARR_ACCESS
				// evaluation just above.
				lhs_addr = getNewTempVar(ADDR);
			    }

			    if(ntype == ADD || ntype == SUB)
				evalAddOp(a->r);
			    else
				evalMulOp(a->r);

			    rhs_addr = result->VMQ_loc;

			    if(result->type == INT)
			    {
				if(l_val->var_type == INT)	op_code = 'i';
				else				op_code = 'F';
			    }
			    else // result->type == FLOAT
			    {
				if(l_val->var_type == FLOAT)	op_code = 'I';
				else				op_code = 'f';
			    }
			    
			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c /-%d %s%d", op_code, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    if(a->l->nodetype == ARR_ACCESS)
				freeTempVar();

			    break;
	
	// Place value of array element into l_val variable.
	case ARR_ACCESS:    evalArrAccess(a->l);
			    lhs_addr = getNewTempVar(ADDR);
			    evalArrAccess(a->r);
			    rhs_addr = getNewTempVar(ADDR);
			    // evalArrAccess will write the commands that store the address of the array element in the first temporary variable.
			    
			    r_node = ((struct var_node*)a->r->l)->val;
			    r_val = r_node->val;
			    
			    if(l_val->var_type == r_val->var_type)
			    {
				if(l_val->var_type == INT)  op_code = 'i';
				else			    op_code = 'I';
			    }
			    else
			    {
				if(l_val->var_type == INT)  op_code = 'f';
				else			    op_code = 'F';
			    }

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "%c @/-%d %s%d", op_code, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    freeTempVar();
			    freeTempVar();

			    break;

	// Place return value of function into l_val variable.
	case FUNC_CALL:	    if(a->l->nodetype == ARR_ACCESS)	
			    {
				evalArrAccess(a->l);
				lhs_addr = getNewTempVar(ADDR);
				VMQ_addr_l_prefix = "/-";
			    }
			    else    // Function semantics are a bit different, so adjust memory access mode as needed.
			    {
				if(l_val->isGlobal)	VMQ_addr_l_prefix = "#";
				else if(l_val->isParam)	VMQ_addr_l_prefix = "/";
				else			VMQ_addr_l_prefix = "#/-";
			    }

			    func = ((struct func_node*)a->r->l)->val;
			    evalFuncCall(a->r, func->return_type); // Generates the appropriate statements for pushing variables onto the stack for passing.
			    
			    if(l_val->var_type == func->return_type)
			    {// We can store the return value of the function directly into the l_val
				VMQ_line = malloc(32);
				sprintf(VMQ_line, "c %s%d %d", VMQ_addr_l_prefix, lhs_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
				sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
				appendToVMQList(VMQ_line);
				free(VMQ_line);
			    }
			    else
			    {// We have to store into a temporary variable and then cast the result into the l_val
				if(l_val->var_type == INT)  op_code = 'f';
				else			    op_code = 'F';

				unsigned int temp_addr;
				
				if(func->return_type == INT)	
				    temp_addr = getNewTempVar(INT);
				else
				    temp_addr = getNewTempVar(FLOAT);

				VMQ_line = malloc(32);
				sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
				sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
				appendToVMQList(VMQ_line);
				sprintf(VMQ_line, "%c /-%d %s%d", op_code, temp_addr, VMQ_addr_l_prefix, lhs_addr);
				appendToVMQList(VMQ_line);
				free(VMQ_line);

				freeTempVar();
			    }

			    if(a->l->nodetype == ARR_ACCESS)
				freeTempVar();
			    
			    break;
    }

    while(CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
	freeTempVar();
}

void evalIncOp(struct AST_node* a)
{
    if(!a || a->nodetype == 0)	return;
    if(!a->r || a->r->nodetype == 0) return;

    // Supporting data structures, shortens code in various switch cases.
    char op_code;
    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    struct func_list_node* func = NULL;
    struct varref* l_node = NULL, *r_node = NULL;
    struct var* l_val = NULL, *r_val = NULL;
    struct intlit* i_lit = NULL;
    struct fltlit* f_lit = NULL;

    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    unsigned int lhs_type, lhs_addr, rhs_addr;
    unsigned int temp_addr = 0;

    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

	// Initial setup for LHS data (LHS is a VAR_ACCESS or ARR_ACCESS)
    if(a->l->nodetype == VAR_ACCESS)
    {
		l_node = ((struct var_node*)a->l)->val;
		l_val = l_node->val;

		if(l_val->isGlobal)	VMQ_addr_l_prefix = "";
		else if(l_val->isParam)	VMQ_addr_l_prefix = "@/";
		else			VMQ_addr_l_prefix = "/-";

		lhs_type = VAR_ACCESS;
		lhs_addr = l_node->VMQ_loc;
    }
    else // a->l->nodetype == ARR_ACCESS
    {
		l_node = ((struct var_node*)a->l->l)->val;
		l_val = l_node->val;
		
		VMQ_addr_l_prefix = "@/-";
		lhs_type = ARR_ACCESS;
    }

	// a->l is guaranteed to be a variable - expr may need to be evaluated.
	struct AST_node* expr = a->r;
    int ntype = expr->nodetype;

    switch(ntype)
    {
	/* 
	    Cases:  ASSIGNOP, ADD_ASSIGN, SUB_ASSIGN, ADD, SUB, MUL, DIV, MOD
		    VAR_ACCESS, FUNC_CALL, INT_LITERAL, FLT_LITERAL, ARR_ACCESS
	*/
	case ASSIGNOP:		
	case ADD_ASSIGN:	
	case SUB_ASSIGN:	if(ntype == ASSIGNOP)	
				    evalAssignOp(expr);
				else
				    evalIncOp(expr);

				if(expr->l->nodetype == VAR_ACCESS)
				{
				    r_node = ((struct var_node*)expr->l)->val;
				    r_val = r_node->val;

				    if(r_val->isGlobal)		VMQ_addr_r_prefix = "";
				    else if(r_val->isParam)	VMQ_addr_r_prefix = "@/";
				    else			VMQ_addr_r_prefix = "/-";

				    rhs_addr = r_node->VMQ_loc;
				}
				else // expr->l->nodetype == ARR_ACCESS
				{
				    r_node = ((struct var_node*)expr->l->l)->val;
				    r_val = r_node->val;

				    rhs_addr = getNewTempVar(ADDR);
				    VMQ_addr_r_prefix = "@/-";
				}

				if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				VMQ_line = malloc(32);
				if(l_val->var_type == r_val->var_type)
				{
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);
				}
				else	// Casting is required.
				{
				    if(l_val->var_type == INT)
				    {
					op_code = 'f';
					temp_addr = getNewTempVar(INT);
				    }
				    else
				    {
					op_code = 'F';
					temp_addr = getNewTempVar(FLOAT);
				    }

				    sprintf(VMQ_line, "%c %s%d /-%d", op_code, VMQ_addr_r_prefix, rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);
								
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);

				    freeTempVar();
				}

				if(lhs_type == ARR_ACCESS)
				    freeTempVar();
				
				if(expr->l->nodetype == ARR_ACCESS)
				    freeTempVar();

				free(VMQ_line);

				break;

	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:		if(ntype == ADD || ntype == SUB)
				    evalAddOp(expr);
				else
				    evalMulOp(expr);

				rhs_addr = result->VMQ_loc;

				if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				VMQ_line = malloc(32);
				if(l_val->var_type == result->type)
				{
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);
				}
				else
				{
				    if(l_val->var_type == INT)
					op_code = 'f';
				    else
					op_code = 'F';

				    if(op_code == 'F' && (temp_addr % VMQ_FLT_SIZE != 0))
					temp_addr = getNewTempVar(FLOAT);
				    else
					temp_addr = rhs_addr;

				    sprintf(VMQ_line, "%c /-%d /-%d", op_code, rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);

				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);

				    if(temp_addr != rhs_addr)
					freeTempVar();
				}

				if(lhs_type == ARR_ACCESS)
				    freeTempVar();

				free(VMQ_line);

				break;

	case VAR_ACCESS:	if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				r_node = ((struct var_node*)expr)->val;
				r_val = r_node->val;

				rhs_addr = r_node->VMQ_loc;

				if(r_val->isGlobal)	VMQ_addr_r_prefix = "";
				else if(r_val->isParam)	VMQ_addr_r_prefix = "@/";
				else			VMQ_addr_r_prefix = "/-";

				VMQ_line = malloc(32);
				if(l_val->var_type == r_val->var_type)
				{
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);
				}
				else
				{
				    if(l_val->var_type == INT)
				    {
					op_code = 'f';
					temp_addr = getNewTempVar(INT);
				    }
				    else
				    {
					op_code = 'F';
					temp_addr = getNewTempVar(FLOAT);
				    }

				    sprintf(VMQ_line, "%c %s%d /-%d", op_code, VMQ_addr_r_prefix, rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);
								
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);

				    freeTempVar();
				}

				if(lhs_type == ARR_ACCESS)
				    freeTempVar();

				free(VMQ_line);
			    
				break;

	case FUNC_CALL:		func = ((struct func_node*)expr)->val;
				evalFuncCall(expr, func->return_type);
				

				if(func->return_type == INT)
				    rhs_addr = temp_addr = getNewTempVar(INT);
				else
				    rhs_addr = temp_addr = getNewTempVar(FLOAT);

				VMQ_line = malloc(32);

				sprintf(VMQ_line, "c #/-%d %d", rhs_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);

				if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				if(l_val->var_type == func->return_type)
				{
				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);
				}
				else
				{
				    op_code = (l_val->var_type == INT) ? 'f' : 'F';
				    if(op_code == 'F' && (temp_addr % VMQ_FLT_SIZE != 0))
					rhs_addr = getNewTempVar(FLOAT);

				    sprintf(VMQ_line, "%c /-%d /-%d", op_code, temp_addr, rhs_addr);
				    appendToVMQList(VMQ_line);

				    if(a->nodetype == ADD_ASSIGN)
					op_code = (l_val->var_type == INT) ? 'a' : 'A';	
				    else // SUB_ASSIGN INCOP
					op_code = (l_val->var_type == INT) ? 's' : 'S';

				    sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				    appendToVMQList(VMQ_line);

				    if(rhs_addr != temp_addr)
					freeTempVar();
				}

				if(lhs_type == ARR_ACCESS)
				    freeTempVar();

				freeTempVar();

				free(VMQ_line);

				break;

	case INT_LITERAL:	i_lit = ((struct int_node*)expr)->val;
				rhs_addr = i_lit->VMQ_loc;

				if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				VMQ_line = malloc(32);
				if(l_val->var_type != INT)
				{
				    temp_addr = getNewTempVar(FLOAT);
				    sprintf(VMQ_line, "F %d /-%d", rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);
				    rhs_addr = temp_addr;
				    VMQ_addr_r_prefix = "/-";
				}
				else
				    VMQ_addr_r_prefix = "";

				if(a->nodetype == ADD_ASSIGN)
				    op_code = (l_val->var_type == INT) ? 'a' : 'A';	    
				else // SUB_ASSIGN INCOP
				    op_code = (l_val->var_type == INT) ? 's' : 'S';

				sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				appendToVMQList(VMQ_line);

				if(rhs_addr != i_lit->VMQ_loc)
				    freeTempVar();
			    
				if(lhs_type == ARR_ACCESS)
				    freeTempVar();

				free(VMQ_line);

				break;

	case FLT_LITERAL:	f_lit = ((struct flt_node*)expr)->val;
				rhs_addr = f_lit->VMQ_loc;

				if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				VMQ_line = malloc(32);
				if(l_val->var_type != FLOAT)
				{
				    temp_addr = getNewTempVar(INT);
				    sprintf(VMQ_line, "f %d /-%d", rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);
				    rhs_addr = temp_addr;
				    VMQ_addr_r_prefix = "/-";
				}
				else
				    VMQ_addr_r_prefix = "";

				if(a->nodetype == ADD_ASSIGN)
				    op_code = (l_val->var_type == INT) ? 'a' : 'A';	    
				else // SUB_ASSIGN INCOP
				    op_code = (l_val->var_type == INT) ? 's' : 'S';

				sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				appendToVMQList(VMQ_line);

				if(rhs_addr != f_lit->VMQ_loc)
				    freeTempVar();
			    
				if(lhs_type == ARR_ACCESS)
				    freeTempVar();

				free(VMQ_line);

				break;

	case ARR_ACCESS:	if(lhs_type == ARR_ACCESS)
				{
				    evalArrAccess(a->l);
				    lhs_addr = getNewTempVar(ADDR);
				}

				evalArrAccess(expr);
				rhs_addr = getNewTempVar(ADDR);

				VMQ_addr_r_prefix = "@/-";
				r_node = ((struct var_node*)expr->l)->val;
				r_val = r_node->val;

				VMQ_line = malloc(32);

				if(l_val->var_type != r_val->var_type)
				{
				    if(l_val->var_type == INT)
				    {
					op_code = 'f';
					temp_addr = getNewTempVar(INT);
				    }
				    else
				    {
					op_code = 'F';
					temp_addr = getNewTempVar(FLOAT);
				    }

				    sprintf(VMQ_line, "%c @/-%d /-%d", op_code, rhs_addr, temp_addr);
				    appendToVMQList(VMQ_line);

				    rhs_addr = temp_addr;
				    VMQ_addr_r_prefix = "/-";
				}

				if(a->nodetype == ADD_ASSIGN)
				    op_code = (l_val->var_type == INT) ? 'a' : 'A';	    
				else // SUB_ASSIGN INCOP
				    op_code = (l_val->var_type == INT) ? 's' : 'S';

				sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
				appendToVMQList(VMQ_line);

				if(temp_addr)
				    freeTempVar();

				// RHS array ADDR storage
				freeTempVar();

				if(lhs_type == ARR_ACCESS)
				    freeTempVar();
							
				break;
    }

    while(CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
	freeTempVar();
}

void evalArrAccess(struct AST_node* a)
{
    if(!a || a->nodetype == 0)
	yyerror("evalArrAccess() encountered NULL or invalid \"a\"  nodetype");
    if(!a->r || a->r->nodetype == 0)
	yyerror("evalArrAccess() encountered NULL or invalid \"a->\" nodetype");

    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    struct func_list_node* func = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    struct varref* l_node = ((struct var_node*)a->l)->val, *r_node = NULL;
    struct var* l_val = l_node->val, *r_val = NULL; 
    unsigned int lit_loc;

    // Size of VMQ int data type is stored at global memory addr 4
    // Size of VMQ float data type is stored at global memory addr 6
    char type_size_addr = (l_val->var_type == INT) ? '4' : '6';

    if(l_val->isGlobal)	    VMQ_addr_l_prefix = "#";
    else if(l_val->isParam) VMQ_addr_l_prefix = "/";
    else		    VMQ_addr_l_prefix = "#/-";

    // expr must be evaluated to determine array index
    struct AST_node* expr = a->r;

    unsigned int ntype = expr->nodetype;
    switch(ntype)
    {
	// Evaluate assignment-based op, use result to calculate index.
	// r_val is either a variable or an array element
	case ASSIGNOP:
	case INCOP:	    if(ntype == ASSIGNOP)   
				evalAssignOp(expr);
			    else
				evalIncOp(expr);
			    
			    VMQ_line = malloc(32);

			    if(expr->l->nodetype == VAR_ACCESS)
			    {
				r_node = ((struct var_node*)expr->l)->val;
				r_val = r_node->val;

				if(r_val->var_type == FLOAT) yyerror("Floating point value used for array index");

				if(r_val->isGlobal)	VMQ_addr_r_prefix = "";
				else if(r_val->isParam)	VMQ_addr_r_prefix = "@/";
				else			VMQ_addr_r_prefix = "/-";

				result->VMQ_loc = getNewTempVar(ADDR);

				// Generates stmt that will calculate the offset from the array base addr, stored in temp_var
				sprintf(VMQ_line, "m %s%d %c /-%d", VMQ_addr_r_prefix, r_node->VMQ_loc, type_size_addr, result->VMQ_loc);
				appendToVMQList(VMQ_line);
				// Stores the final address of the l_val array element address, storing it temp_var
				sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
				appendToVMQList(VMQ_line);
			    }
			    else // expr->l->nodetype == ARR_ACCESS
			    {
				r_node = ((struct var_node*)expr->l->l)->val;
				r_val = r_node->val;

				if(r_val->var_type == FLOAT) yyerror("Floating point value used for array index");

				result->VMQ_loc = getNewTempVar(ADDR);
				
				// The first operand (@/-%d) == retrieving the value from the address of r_val's array element 
				// that was calculated as a part of evalAssignOp() or evalIncOp().	
				sprintf(VMQ_line, "m @/-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
				appendToVMQList(VMQ_line);

				sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
				appendToVMQList(VMQ_line);
			    }

			    free(VMQ_line);
			    freeTempVar();
			
			    break;

	// Evaluate math op, use result to calculate index.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    if(ntype == ADD || ntype == SUB)
				evalAddOp(expr);
			    else
				evalMulOp(expr);

			    if(result->type == FLOAT) 
				yyerror("evalArrAccess() - Floating point value used for array index");

			    VMQ_line = malloc(32);

			    // Generates stmt that will calculate the offset from the array base addr, store in temp_var.
			    sprintf(VMQ_line, "m /-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    // Generates final stmt for calculating array element addr, store in temp_var.
			    sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    break;

	case VAR_ACCESS:    r_node = ((struct var_node*)expr)->val;
			    r_val = r_node->val;

			    if(r_val->var_type == FLOAT) yyerror("Floating point value used for array index");

			    if(r_val->isGlobal)		VMQ_addr_r_prefix = "";
			    else if(r_val->isParam)	VMQ_addr_r_prefix = "@/";
			    else			VMQ_addr_r_prefix = "/-";

			    result->VMQ_loc = getNewTempVar(ADDR);

			    VMQ_line = malloc(32);

			    // Generates stmt that will calculate the offset from the array base addr, stored in temp_var
			    sprintf(VMQ_line, "m %s%d %c /-%d", VMQ_addr_r_prefix, r_node->VMQ_loc, type_size_addr, result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    // Stores the final address of the l_val array element address, storing it temp_var
			    sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    freeTempVar();
			    free(VMQ_line);
				
			    break;

	case FUNC_CALL:	    func = ((struct func_node*)expr->l)->val;

			    if(func->return_type == FLOAT) 
				yyerror("Floating point value used for array index");
			    
			    // Push parameters onto the stack
			    evalFuncCall(expr, func->return_type);
			    
			    result->VMQ_loc = getNewTempVar(ADDR);

			    VMQ_line = malloc(32);

			    // The index will be the result of the function call.
			    sprintf(VMQ_line, "c #/-%d %d", result->VMQ_loc, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			    
			    sprintf(VMQ_line, "m /-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc); 
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    free(VMQ_line);

			    break;
			
	case INT_LITERAL:   
	case FLT_LITERAL:   if(ntype == INT_LITERAL)
				lit_loc = ((struct int_node*)expr)->val->VMQ_loc;
			    else
				yyerror("Floating point value used for array index");

			    result->VMQ_loc = getNewTempVar(ADDR);
			    
			    VMQ_line = malloc(32);

			    sprintf(VMQ_line, "m %d %c /-%d", lit_loc, type_size_addr, result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    freeTempVar();
			    free(VMQ_line);

			    break;

	case ARR_ACCESS:    evalArrAccess(expr);
			    
			    VMQ_line = malloc(32);

			    sprintf(VMQ_line, "m @/-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			    appendToVMQList(VMQ_line);

			    free(VMQ_line);

			    break;
    }
}

/*
    This eval function does not actually generate the function call statement itself (i.e., "c <#> <#>").
    This is due to the fact that the 1st operand of the function call statement is used to determine
    where the return value of the function is placed - this is dependent on the context of the function call.
*/
void evalFuncCall(struct AST_node* a, unsigned int return_type)
{
    if(!a || a->nodetype == 0)	return;

    char op_code;
    char* VMQ_line = NULL, *VMQ_addr_prefix = NULL;
    struct func_list_node* func = NULL, *target_func = NULL;
    struct varref* v_node = NULL;
    struct var* v = NULL;
    struct intlit* i_lit = NULL;
    struct fltlit* f_lit = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    struct AST_node* param;

    unsigned int orig_addr, orig_size, temp_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;

    if(a->r && a->nodetype == FUNC_CALL)
	param = a->r;
    else
	param = a;

    // Parameter(s) are any type of expression:
    // ASSIGNOPs, INCOPs, ADDOPs, MULOPs, VAR/ARR_ACCESSes, INT/FLT_LITERALS, or other FUNC_CALLs.
    switch(param->nodetype)
    {
	// Highest level - expression list case.
	case EXPRS:	    orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
			    orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

			    target_func = ((struct func_node*)a->l)->val;
			    evalFuncCall(param->r, target_func->return_type);
			    
			    if(param->r->nodetype == FUNC_CALL)
			    {
				// Code for handling nested function calls (i.e., function calls used as arguments).
				// The nested function call EXPRS have already been evaluated, so we'll just handle the
				// code for generating the appropriate VMQ statements to handle the function call itself.

				func = ((struct func_node*)param->r->l)->val;

				if(func->return_type == INT)	temp_addr = getNewTempVar(INT);
				else				temp_addr = getNewTempVar(FLOAT);

				VMQ_line = malloc(32);
				
				// Call function, store return value in temporary variable
				sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
				// Pop the called functions parameters off of the stack.
				sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
				appendToVMQList(VMQ_line);
				// Push the result of the function call onto the stack.
				sprintf(VMQ_line, "p #/-%d", temp_addr);
				appendToVMQList(VMQ_line);

				free(VMQ_line);
			    }
	
			    evalFuncCall(param->l, target_func->return_type);

			    if(param->l->nodetype == FUNC_CALL)
			    {
				// Code for handling nested function calls (i.e., function calls used as arguments).
				// The nested function call EXPRS have already been evaluated, so we'll just handle the
				// code for generating the appropriate VMQ statements to handle the function call itself.

				func = ((struct func_node*)param->l->l)->val;

				if(func->return_type == INT)    temp_addr = getNewTempVar(INT);
				else				temp_addr = getNewTempVar(FLOAT);

				VMQ_line = malloc(32);
				
				// Call function, store return value in temporary variable
				sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
				// Pop the called functions parameters off of the stack.
				sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
				appendToVMQList(VMQ_line);
				// Push the result of the function call onto the stack.
				sprintf(VMQ_line, "p #/-%d", temp_addr);
				appendToVMQList(VMQ_line);

				free(VMQ_line);
			    }
	
			    while(CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
				freeTempVar();

			    break;
	
	// Lower level - individual evaluation and push statements.

	// Push the address of the l_val/variable onto the stack.
	case ASSIGNOP:	    
	case ADD_ASSIGN:
	case SUB_ASSIGN:    if(param->nodetype == ASSIGNOP)	
				evalAssignOp(param);
			    else   
				evalIncOp(param); 
			    
			    v_node = ((struct var_node*)param->l)->val;
			    v = v_node->val;

	case VAR_ACCESS:    
	case ARR_ACCESS:    if(param->nodetype == VAR_ACCESS)
			    { 
				v_node = ((struct var_node*)param)->val; 
				v = v_node->val;

				if(v->isGlobal)	    VMQ_addr_prefix = "#";
				else if(v->isParam) VMQ_addr_prefix = "/";
				else		    VMQ_addr_prefix = "#/-";
			    }
			    else if(param->nodetype == ARR_ACCESS)
			    {
				evalArrAccess(param);

				result->VMQ_loc = getNewTempVar(ADDR);
				
				v_node = ((struct var_node*)param->l)->val;
				v = v_node->val;

				VMQ_addr_prefix = "/-";
			    }

			    VMQ_line = malloc(32);

			    if(v->var_type == return_type)
			    {
				if(param->nodetype == ARR_ACCESS)
				    sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, result->VMQ_loc);
				else
				    sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, v_node->VMQ_loc);

				appendToVMQList(VMQ_line);
			    }
			    else // need to cast the value to the correct type before pushing.
			    {
				unsigned int src_addr;
				if(param->nodetype == ARR_ACCESS)
				{
				    src_addr = result->VMQ_loc;
				    freeTempVar();
				    VMQ_addr_prefix = "@/-";
				}
				else
				{
				    src_addr = v_node->VMQ_loc;
				    VMQ_addr_prefix = "/-";
				}

				if(return_type == INT)
				{
				    op_code = 'f';
				    temp_addr = getNewTempVar(INT);
				}
				else
				{
				    op_code = 'F';
				    temp_addr = getNewTempVar(FLOAT);
				}

				// Generate the cast statement
				sprintf(VMQ_line, "%c %s%d /-%d", op_code, VMQ_addr_prefix, src_addr, temp_addr);
				appendToVMQList(VMQ_line);
				
				// Generate the push statement
				sprintf(VMQ_line, "p #/-%d", temp_addr);
				appendToVMQList(VMQ_line);
			    }
			     
			    free(VMQ_line);

			    break;
	
	// Push the result of the math op onto the stack.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:	    if(param->nodetype == ADD || param->nodetype == SUB)
				evalAddOp(param);
			    else
				evalMulOp(param);

			    VMQ_line = malloc(32);
			    sprintf(VMQ_line, "p #/-%d", result->VMQ_loc);
			    appendToVMQList(VMQ_line);
			    free(VMQ_line);
			    
			    break;

	// Push the int/float literal onto the stack.
	case INT_LITERAL:
	case FLT_LITERAL:   VMQ_line = malloc(32);
			    if(param->nodetype == INT_LITERAL)
			    {
				i_lit = ((struct int_node*)param)->val;
				sprintf(VMQ_line, "p #%d", i_lit->VMQ_loc);
			    }
			    else
			    {
				f_lit = ((struct flt_node*)param)->val;
				sprintf(VMQ_line, "p #%d", f_lit->VMQ_loc);
			    }

			    appendToVMQList(VMQ_line);
			    free(VMQ_line);

			    break;

	// Push the return value of the function onto the stack.
//	case FUNC_CALL:	    evalFuncCall(param);
//			    
//			    break;
    }
}

void evalAddOp(struct AST_node* a)
{
    if(!a || a->nodetype == 0)	yyerror("evalAddOp() encountered NULL or empty AST node");

    // Supporting data structures
    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    char op_code;
    struct varref* l_node = NULL, *r_node = NULL;
    struct var* l_val = NULL, *r_val = NULL;
    struct intlit* r_int = NULL;
    struct fltlit* r_flt = NULL;
    struct func_list_node* func = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;
    unsigned int result_type = INT;
    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
    unsigned int lhs_addr, rhs_addr, new_addr, dest_addr;

    struct AST_node* lhs = a->l, *rhs = a->r;
    unsigned int lhs_ntype = lhs->nodetype, rhs_ntype;
    
    // For unary math ops, rhs will be NULL.
    if(rhs)
	rhs_ntype = rhs->nodetype;
    else
	rhs_ntype = 0;

    // Used to track if the lhs/rhs is an INT or FLOAT, 
    // so we can cast as needed while evaluating ADDOPs.
    unsigned int lhs_datatype, rhs_datatype;

    unsigned int ntype = a->nodetype;
    switch(ntype)
    {
	case ADD:   
	case SUB:   VMQ_line = malloc(32);
		    if(isMathLeaf(lhs_ntype) || lhs_ntype == FUNC_CALL)
		    {
			if(lhs_ntype == INT_LITERAL)
			{
			    lhs_datatype = INT;
			    VMQ_addr_l_prefix = "";
			    lhs_addr = ((struct int_node*)lhs)->val->VMQ_loc;
			}
			else if(lhs_ntype == FLT_LITERAL)
			{
			    lhs_datatype = FLOAT;
			    VMQ_addr_l_prefix = "";
			    lhs_addr = ((struct flt_node*)lhs)->val->VMQ_loc;
			}
			else if(lhs_ntype == VAR_ACCESS)
			{
			    l_node = ((struct var_node*)lhs)->val;
			    l_val = l_node->val;

			    lhs_datatype = l_val->var_type;

			    if(l_val->isGlobal)	    VMQ_addr_l_prefix = "";
			    else if(l_val->isParam) VMQ_addr_l_prefix = "@/";
			    else		    VMQ_addr_l_prefix = "/-";

			    lhs_addr = l_node->VMQ_loc;
			}
			else if(lhs_ntype == ARR_ACCESS)
			{
			    lhs_datatype = ((struct var_node*)lhs->l)->val->val->var_type;
			    VMQ_addr_l_prefix = "@/-";
			    evalArrAccess(lhs);
			    lhs_addr = getNewTempVar(ADDR);
			}
			else if(lhs_ntype == FUNC_CALL)
			{
			    func = ((struct func_node*)lhs->l)->val;
			    lhs_datatype = func->return_type;
			    VMQ_addr_l_prefix = "/-";
			    evalFuncCall(lhs, func->return_type);
			    lhs_addr = getNewTempVar(lhs_datatype);
			    sprintf(VMQ_line, "c #/-%d %d", lhs_addr, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			}
		    }
		    else
		    {
			if(lhs_ntype == ADD || lhs_ntype == SUB)
			    evalAddOp(lhs);
			else
			    evalMulOp(lhs);

			lhs_datatype = result->type;			
			VMQ_addr_l_prefix = "/-";
			lhs_addr = result->VMQ_loc;
		    }

		    // Special case:  unary add/sub.
		    if(rhs_ntype == 0)
		    {
			// If lhs is the result of a function call or math op evaluation, then we already allocated a
			// temporary variable for it - no need to allocate another for the unary op.
			result->VMQ_loc = new_addr = (isMathLeaf(lhs_ntype) && lhs_ntype != FUNC_CALL) ? getNewTempVar(lhs_datatype) : lhs_addr;
 
			if(lhs_datatype == INT)
			{
			    if(ntype == ADD)
			    {
				// Might as well be a no-op, but we're going for a literal translation.
				sprintf(VMQ_line, "a %s%d 000 /-%d", VMQ_addr_l_prefix, lhs_addr, new_addr);
				appendToVMQList(VMQ_line);
			    }
			    else // ntype == SUB
			    {
				sprintf(VMQ_line, "n %s%d /-%d", VMQ_addr_l_prefix, lhs_addr, new_addr);
				appendToVMQList(VMQ_line);
			    }
			}
			else // lhs_datatype == FLOAT
			{
			    if(ntype == ADD)
			    {
				// This might as well be a no-op, but we're going for a literal translation.
				unsigned int cast_addr = getNewTempVar(FLOAT);
				freeTempVar();
				sprintf(VMQ_line, "F 000 /-%d", cast_addr + VMQ_FLT_SIZE);
				appendToVMQList(VMQ_line);
				sprintf(VMQ_line, "A %s%d /-%d /-%d", VMQ_addr_l_prefix, lhs_addr, cast_addr, new_addr);
				appendToVMQList(VMQ_line);
			    }
			    else // ntype == SUB
			    {
				sprintf(VMQ_line, "N %s%d /-%d", VMQ_addr_l_prefix, lhs_addr, new_addr);
				appendToVMQList(VMQ_line);
			    }
			}

			free(VMQ_line);

			// No need to compute rest of the code below, break out of the switch.
			break;
		    }
		    else if(isMathLeaf(rhs_ntype) || rhs_ntype == FUNC_CALL)
		    {
			if(rhs_ntype == INT_LITERAL)
			{
			    rhs_datatype = INT;
			    VMQ_addr_r_prefix = "";
			    rhs_addr = ((struct int_node*)rhs)->val->VMQ_loc;
			}
			else if(rhs_ntype == FLT_LITERAL)
			{
			    rhs_datatype = FLOAT;
			    VMQ_addr_r_prefix = "";
			    rhs_addr = ((struct flt_node*)rhs)->val->VMQ_loc;
			}
			else if(rhs_ntype == VAR_ACCESS)
			{
			    r_node = ((struct var_node*)rhs)->val;
			    r_val = r_node->val;

			    rhs_datatype = r_val->var_type;

			    if(r_val->isGlobal)	    VMQ_addr_r_prefix = "";
			    else if(r_val->isParam) VMQ_addr_r_prefix = "@/";
			    else		    VMQ_addr_r_prefix = "/-";

			    rhs_addr = r_node->VMQ_loc;
			}
			else if(rhs_ntype == ARR_ACCESS)
			{
			    rhs_datatype = ((struct var_node*)rhs->l)->val->val->var_type;
			    VMQ_addr_r_prefix = "@/-";
			    evalArrAccess(rhs);
			    rhs_addr = getNewTempVar(ADDR);
			}
			else if(rhs_ntype == FUNC_CALL)
			{
			    func = ((struct func_node*)rhs->l)->val;
			    rhs_datatype = func->return_type;
			    VMQ_addr_r_prefix = "/-";
			    evalFuncCall(rhs, func->return_type);
			    rhs_addr = getNewTempVar(rhs_datatype);
			    sprintf(VMQ_line, "c #/-%d %d", rhs_addr, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			}
		    }
		    else // rhs is some math op, needs to be evaluated
		    {
			if(rhs_ntype == ADD || rhs_ntype == SUB)
			    evalAddOp(rhs);
			else
			    evalMulOp(rhs);
			rhs_datatype = result->type;
			VMQ_addr_r_prefix = "/-";
			rhs_addr = result->VMQ_loc;
		    }

		    // Casting is required before generating the ADD/SUB operation.
		    if(lhs_datatype != rhs_datatype)
		    {
			unsigned int* addr = (lhs_datatype == INT) ? &lhs_addr : &rhs_addr;
			unsigned int* ntype_ptr = (lhs_datatype == INT) ? &lhs_ntype : &rhs_ntype;
			unsigned int* datatype = (lhs_datatype == INT) ? &lhs_datatype : &rhs_datatype;
			char** prefix = (lhs_datatype == INT) ? &VMQ_addr_l_prefix : &VMQ_addr_r_prefix;
			
			unsigned int temp_start_addr = CURRENT_FUNC->VMQ_data.tempvar_start;

			if(isMathLeaf(*ntype_ptr))
			    new_addr = getNewTempVar(FLOAT);
			else
			{
			    if(*addr == temp_start_addr)
				new_addr = *addr;
			    else
				new_addr = getNewTempVar(FLOAT);
			}

			sprintf(VMQ_line, "F %s%d /-%d", *prefix, *addr, new_addr);
			appendToVMQList(VMQ_line);
			*prefix = "/-";
			*addr = new_addr;

			result->type == FLOAT;

			op_code = (ntype == ADD) ? 'A' : 'S';

			if(isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			    result->VMQ_loc = new_addr;
			else if(isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = (rhs_datatype == FLOAT) ? rhs_addr : new_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();
				freeTempVar();
			    }
			}
			else if(!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = (lhs_datatype == FLOAT) ? lhs_addr : new_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();
				freeTempVar();
			    }
			}
			else
			{
			    result->VMQ_loc = (lhs_addr < rhs_addr) ? lhs_addr : rhs_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();	// Need to cleanup two temps here, one for the
				freeTempVar();	// temp storing the other addr used in the op
						// and another for the cast temp created
			    }
			}
			sprintf(VMQ_line, "%c %s%d %s%d /-%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		    }
		    else
		    {
			if(ntype == ADD)
			    op_code = (lhs_datatype == INT) ? 'a' : 'A';
			else
			    op_code = (lhs_datatype == INT) ? 's' : 'S';

			result->type = lhs_ntype;

			if(isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			    result->VMQ_loc = getNewTempVar(lhs_datatype);
			else if(isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
			    result->VMQ_loc = rhs_addr;
			else if(!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			    result->VMQ_loc = lhs_addr;
			else
			{
			    result->VMQ_loc = (lhs_addr < rhs_addr) ? lhs_addr : rhs_addr;
			    freeTempVar();
			}
			sprintf(VMQ_line, "%c %s%d %s%d /-%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		    }

		    free(VMQ_line);

		    break;
    }

    if(lhs_datatype == FLOAT || rhs_datatype == FLOAT)
	result->type = FLOAT;
    else
	result->type = INT;
}

void evalMulOp(struct AST_node* a)
{
    if(!a || a->nodetype == 0)	yyerror("evalAddOp() encountered NULL or empty AST node");

    // Supporting data structures
    char* VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
    char op_code;
    struct varref* l_node = NULL, *r_node = NULL;
    struct var* l_val = NULL, *r_val = NULL;
    struct intlit* r_int = NULL;
    struct fltlit* r_flt = NULL;
    struct func_list_node* func = NULL;
    struct VMQ_temp_node* result = &CURRENT_FUNC->VMQ_data.math_result;

    unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
    unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
    unsigned int lhs_addr, rhs_addr, new_addr, dest_addr;

    struct AST_node* lhs = a->l, *rhs = a->r;
    unsigned int lhs_ntype = lhs->nodetype, rhs_ntype = rhs->nodetype;
    
    // Used to track if the lhs/rhs is an INT or FLOAT, 
    // so we can cast as needed while evaluating ADDOPs.
    unsigned int lhs_datatype, rhs_datatype;

    unsigned int ntype = a->nodetype;

    switch(ntype)
    {
	case MUL:
	case DIV:
	case MOD:   VMQ_line = malloc(32);

		    if(isMathLeaf(lhs_ntype) || lhs_ntype == FUNC_CALL)
		    {
			if(lhs_ntype == INT_LITERAL)
			{
			    lhs_datatype = INT;
			    VMQ_addr_l_prefix = "";
			    lhs_addr = ((struct int_node*)lhs)->val->VMQ_loc;
			}
			else if(lhs_ntype == FLT_LITERAL)
			{
			    lhs_datatype = FLOAT;
			    VMQ_addr_l_prefix = "";
			    lhs_addr = ((struct flt_node*)lhs)->val->VMQ_loc;
			}
			else if(lhs_ntype == VAR_ACCESS)
			{
			    l_node = ((struct var_node*)lhs)->val;
			    l_val = l_node->val;

			    lhs_datatype = l_val->var_type;

			    if(l_val->isGlobal)	    VMQ_addr_l_prefix = "";
			    else if(l_val->isParam) VMQ_addr_l_prefix = "@/";
			    else		    VMQ_addr_l_prefix = "/-";

			    lhs_addr = l_node->VMQ_loc;
			}
			else if(lhs_ntype == ARR_ACCESS)
			{
			    lhs_datatype = ((struct var_node*)lhs->l)->val->val->var_type;
			    VMQ_addr_l_prefix = "@/-";
			    evalArrAccess(lhs);
			    lhs_addr = getNewTempVar(ADDR);
			}
			else if(lhs_ntype == FUNC_CALL)
			{
			    func = ((struct func_node*)lhs->l)->val;
			    lhs_datatype = func->return_type;
			    VMQ_addr_l_prefix = "/-";
			    evalFuncCall(lhs, func->return_type);
			    lhs_addr = getNewTempVar(lhs_datatype);
			    sprintf(VMQ_line, "c #/-%d %d", lhs_addr, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			}
		    }
		    else
		    {
			if(lhs_ntype == ADD || lhs_ntype == SUB)
			    evalAddOp(lhs);
			else
			    evalMulOp(lhs);

			lhs_datatype = result->type;
			VMQ_addr_l_prefix = "/-";
			lhs_addr = result->VMQ_loc;
		    }

		    if(isMathLeaf(rhs_ntype) || rhs_ntype == FUNC_CALL)
		    {
			if(rhs_ntype == INT_LITERAL)
			{
			    rhs_datatype = INT;
			    VMQ_addr_r_prefix = "";
			    rhs_addr = ((struct int_node*)rhs)->val->VMQ_loc;
			}
			else if(rhs_ntype == FLT_LITERAL)
			{
			    rhs_datatype = FLOAT;
			    VMQ_addr_r_prefix = "";
			    rhs_addr = ((struct flt_node*)rhs)->val->VMQ_loc;
			}
			else if(rhs_ntype == VAR_ACCESS)
			{
			    r_node = ((struct var_node*)rhs)->val;
			    r_val = r_node->val;

			    rhs_datatype = r_val->var_type;

			    if(r_val->isGlobal)	    VMQ_addr_r_prefix = "";
			    else if(r_val->isParam) VMQ_addr_r_prefix = "@/";
			    else		    VMQ_addr_r_prefix = "/-";

			    rhs_addr = r_node->VMQ_loc;
			}
			else if(rhs_ntype == ARR_ACCESS)
			{
			    rhs_datatype = ((struct var_node*)rhs->l)->val->val->var_type;
			    VMQ_addr_r_prefix = "@/-";
			    evalArrAccess(rhs);
			    rhs_addr = getNewTempVar(ADDR);
			}
			else if(rhs_ntype == FUNC_CALL)
			{
			    func = ((struct func_node*)rhs->l)->val;
			    rhs_datatype = func->return_type;
			    VMQ_addr_r_prefix = "/-";
			    evalFuncCall(rhs, func->return_type);
			    rhs_addr = getNewTempVar(rhs_datatype);
			    sprintf(VMQ_line, "c #/-%d %d", rhs_addr, func->VMQ_data.quad_start_line);
			    appendToVMQList(VMQ_line);
			}
		    }
		    else // rhs is some math op, needs to be evaluated
		    {
			if(rhs_ntype == ADD || rhs_ntype == SUB)
			    evalAddOp(rhs);
			else
			    evalMulOp(rhs);

			rhs_datatype = result->type;
			VMQ_addr_r_prefix = "/-";
			rhs_addr = result->VMQ_loc;
		    }

		    // Casting is required before generating the MUL/DIV operation.
		    if(lhs_datatype != rhs_datatype)
		    {
			if(ntype == MOD)
			    yyerror("evalMulOp() - operand for residue (modulo) op cannot be float value");

			unsigned int* addr = (lhs_datatype == INT) ? &lhs_addr : &rhs_addr;
			unsigned int* ntype_ptr = (lhs_datatype == INT) ? &lhs_ntype : &rhs_ntype;
			unsigned int* datatype = (lhs_datatype == INT) ? &lhs_datatype : &rhs_datatype;
			char** prefix = (lhs_datatype == INT) ? &VMQ_addr_l_prefix : &VMQ_addr_r_prefix;
			
			unsigned int temp_start_addr = CURRENT_FUNC->VMQ_data.tempvar_start;
   
			if(isMathLeaf(*ntype_ptr))
			    new_addr = getNewTempVar(FLOAT);
			else
			{
			    if(*addr == temp_start_addr)
				new_addr = *addr;
			    else
				new_addr = getNewTempVar(FLOAT);
			}

			sprintf(VMQ_line, "F %s%d /-%d", *prefix, *addr, new_addr);
			appendToVMQList(VMQ_line);
			*prefix = "/-";
			*addr = new_addr;

			result->type = FLOAT;

			if(ntype == MUL)	op_code = 'M';
			else if(ntype == DIV)	op_code = 'D';

			if(isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			    result->VMQ_loc = new_addr;
			else if(isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = (rhs_datatype == FLOAT) ? rhs_addr : new_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();
				freeTempVar();
			    }
			}
			else if(!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = (lhs_datatype == FLOAT) ? lhs_addr : new_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();
				freeTempVar();
			    }
			}
			else
			{
			    result->VMQ_loc = (lhs_addr < rhs_addr) ? lhs_addr : rhs_addr;
			    if(*addr != temp_start_addr)
			    {
				freeTempVar();	// Need to cleanup two temps here, one for the
				freeTempVar();	// temp storing the other addr used in the op
						// and another for the cast temp created.
			    }
			}

			sprintf(VMQ_line, "%c %s%d %s%d /-%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		    }
		    else
		    {
			if(ntype == MUL)
			    op_code = (lhs_datatype == INT) ? 'm' : 'M';
			else if(ntype == DIV)
			    op_code = (lhs_datatype == INT) ? 'd' : 'D';
			else if(ntype == MOD)
			{
			    if(lhs_datatype == FLOAT)
				yyerror("evalMulOp() - operand for residue (modulo) op cannot be float value");

			    op_code = 'r';
			}

			result->type = lhs_datatype;

			if(isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			    result->VMQ_loc = getNewTempVar(lhs_datatype);
			else if(isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = rhs_addr;
			    if(rhs_ntype == FUNC_CALL)
				freeTempVar();
			}
			else if(!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			{
			    result->VMQ_loc = lhs_addr;
			    if(lhs_ntype == FUNC_CALL)
				freeTempVar();
			}
			else
			{
			    result->VMQ_loc = (lhs_addr < rhs_addr) ? lhs_addr : rhs_addr;
			    freeTempVar();
			}

			sprintf(VMQ_line, "%c %s%d %s%d /-%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		    }

		    free(VMQ_line);

		    break;
    }

    if(lhs_datatype == FLOAT || rhs_datatype == FLOAT)
	result->type = FLOAT;
    else
	result->type = INT;
}
