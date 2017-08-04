#include "eval.h"

void evalOutput(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	// Supporting data structures.
	char *addr_mode = NULL;
	struct var *v = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	struct func_list_node *func = NULL;
	unsigned int temp_addr;

	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

	switch (a->nodetype)
	{
	// High level cases - Traverse the subtrees.
	case OUTPUT:
	case STREAMOUT:
		evalOutput(a->l);
		evalOutput(a->r);
		break;

	// Terminal cases - directly output string, integer, or float value.
	case STR_LITERAL:
	case ENDL:
		sprintf(VMQ_line, "p #%d", ((struct str_node *)a)->val->VMQ_loc);
		appendToVMQList(VMQ_line);
		
		appendToVMQList("c 0 -11");
		appendToVMQList("^ 2");
		break;

	case INT_LITERAL:
		sprintf(VMQ_line, "p #%d", ((struct int_node *)a)->val->VMQ_loc);
		appendToVMQList(VMQ_line);
		
		appendToVMQList("c 0 -9");
		appendToVMQList("^ 2");
		break;

	case FLT_LITERAL:		
		sprintf(VMQ_line, "p #%d", ((struct flt_node *)a)->val->VMQ_loc);
		appendToVMQList(VMQ_line);
		
		appendToVMQList("c 0 -10");
		appendToVMQList("^ 2");
		break;

	// Expression cases:
	// ASSIGNOP, ADD_ASSIGN, SUB_ASSIGN, ADD, SUB, MUL, DIV, MOD, FUNC_CALL, VAR_ACCESS, ARR_ACCESS

	// Output value of variable, possibly used in ASSIGNOP/INCOP expression
	case ASSIGNOP:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
		if (a->nodetype == ASSIGNOP)
			evalAssignOp(a);
		else
			evalIncOp(a);
	case VAR_ACCESS:
	case ARR_ACCESS:
		if (a->nodetype == VAR_ACCESS)
			v = ((struct var_node *)a)->val;
		else if (a->nodetype == ARR_ACCESS)
		{
			v = ((struct var_node *)a->l)->val;
			evalArrAccess(a);
		}
		else // a->nodetype == ASSIGNOP || ADD_ASSIGN || SUB_ASSIGN
		{
			if (a->l->nodetype == VAR_ACCESS)
				v = ((struct var_node *)a->l)->val;
			else // a->l->nodetype == ARR_ACCESS
				v = ((struct var_node *)a->l->l)->val;
		}

		if (v->size == 1)
		{
			if (v->isGlobal)
				addr_mode = "#";
			else if (v->isParam)
				addr_mode = "/";
			else
				addr_mode = "#/-";

			sprintf(VMQ_line, "p %s%d", addr_mode, v->VMQ_loc);
		}
		else // v->size > 1 (i.e., var is an array element)
			sprintf(VMQ_line, "p /-%d", result->VMQ_loc);

		appendToVMQList(VMQ_line);
		
		if (v->var_type == INT)
			appendToVMQList("c 0 -9");
		else
			appendToVMQList("c 0 -10");
		appendToVMQList("^ 2");
		break;

	// Output result of math operation.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
		evalMath(a);

		sprintf(VMQ_line, "p #/-%d", result->VMQ_loc);
		appendToVMQList(VMQ_line);		

		if (result->type == INT)
			appendToVMQList("c 0 -9");
		else
			appendToVMQList("c 0 -10");

		appendToVMQList("^ 2");

		break;

	// Output return value of function call.
	case FUNC_CALL:
		func = ((struct func_node *)a->l)->val;
		evalFuncCall(a, func->param_list_tail);

		// Store return value of function into a temporary variable.
		if (func->return_type == INT)
			temp_addr = getNewTempVar(INT);
		else
			temp_addr = getNewTempVar(FLOAT);

		sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "p #/-%d", temp_addr);
		appendToVMQList(VMQ_line);

		freeTempVar();

		if (func->return_type == INT)
			appendToVMQList("c 0 -9");
		else
			appendToVMQList("c 0 -10");

		appendToVMQList("^ 2");

		break;

	// No other nodetypes should be encountered - output error and quit if one is encountered.
	default:
		yyerror("evalOutput() - Unknown nodetype encountered");
	}

	while (orig_size != CURRENT_FUNC->VMQ_data.tempvar_cur_size)
		freeTempVar();
}
