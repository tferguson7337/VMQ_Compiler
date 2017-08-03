#include "eval.h"

void evalOutput(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	// Supporting data structures.
	char *VMQ_line = NULL, *VMQ_addr_prefix = NULL;
	struct var *v = NULL;
	struct varref *vnode = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	struct func_list_node *func = NULL;
	unsigned int temp_addr;

	unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
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
		VMQ_line = malloc(32);
		sprintf(VMQ_line, "p #%d", ((struct str_node *)a)->val->VMQ_loc);
		appendToVMQList(VMQ_line);
		free(VMQ_line);
		appendToVMQList("c 0 -11");
		appendToVMQList("^ 2");
		break;

	case INT_LITERAL:
		VMQ_line = malloc(32);
		sprintf(VMQ_line, "p #%d", ((struct int_node *)a)->val->VMQ_loc);
		appendToVMQList(VMQ_line);
		free(VMQ_line);
		appendToVMQList("c 0 -9");
		appendToVMQList("^ 2");
		break;

	case FLT_LITERAL:
		VMQ_line = malloc(32);
		sprintf(VMQ_line, "p #%d", ((struct flt_node *)a)->val->VMQ_loc);
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
	case SUB_ASSIGN:
		if (a->nodetype == ASSIGNOP)
			evalAssignOp(a);
		else
			evalIncOp(a);
	case VAR_ACCESS:
	case ARR_ACCESS:
		if (a->nodetype == VAR_ACCESS)
		{
			vnode = ((struct var_node *)a)->val;
			v = vnode->val;
		}
		else if (a->nodetype == ARR_ACCESS)
		{
			vnode = ((struct var_node *)a->l)->val;
			v = vnode->val;

			evalArrAccess(a);
		}
		else // a->nodetype == ASSIGNOP || ADD_ASSIGN || SUB_ASSIGN
		{
			if (a->l->nodetype == VAR_ACCESS)
			{
				vnode = ((struct var_node *)a->l)->val;
				v = vnode->val;
			}
			else // a->l->nodetype == ARR_ACCESS
			{
				vnode = ((struct var_node *)a->l->l)->val;
				v = vnode->val;
			}
		}

		VMQ_line = malloc(32);

		if (v->size == 1)
		{
			if (v->isGlobal)
				VMQ_addr_prefix = "#";
			else if (v->isParam)
				VMQ_addr_prefix = "/";
			else
				VMQ_addr_prefix = "#/-";

			sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
		}
		else // v->size > 1 (i.e., var is an array element)
			sprintf(VMQ_line, "p /-%d", result->VMQ_loc);

		appendToVMQList(VMQ_line);
		free(VMQ_line);
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
		if (a->nodetype == ADD || a->nodetype == SUB)
			evalAddOp(a);
		else
			evalMulOp(a);

		VMQ_line = malloc(32);
		sprintf(VMQ_line, "p #/-%d", result->VMQ_loc);
		appendToVMQList(VMQ_line);
		free(VMQ_line);

		if (result->type == INT)
			appendToVMQList("c 0 -9");
		else
			appendToVMQList("c 0 -10");

		appendToVMQList("^ 2");

		break;

	// Output return value of function call.
	case FUNC_CALL:
		func = ((struct func_node *)a->l)->val;
		evalFuncCall(a, func->return_type, func->param_list_tail);

		// Store return value of function into a temporary variable.
		if (func->return_type == INT)
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

		if (func->return_type == INT)
			appendToVMQList("c 0 -9");
		else
			appendToVMQList("c 0 -10");

		appendToVMQList("^ 2");

		break;

	// No other nodetypes should be encountered - output error and quit if one is encountered.
	default:
		yyerror("evalOutput() - Unknown nodetype encountered");
		exit(-1);
	}

	while (orig_size != CURRENT_FUNC->VMQ_data.tempvar_cur_size)
		freeTempVar();
}
