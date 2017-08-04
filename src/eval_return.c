#include "eval.h"

void evalReturn(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	// Supporting data structures.
	char *VMQ_line = NULL, *VMQ_addr_prefix = NULL;
	char op_code;
	struct var *v = NULL;
	struct varref *vnode = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	unsigned int temp_addr, r_val_addr, return_type = CURRENT_FUNC->return_type;

	switch (a->nodetype)
	{
	// left child of RETURN node is any type of expression, right child is guaranteed to be NULL
	case RETURN:
		evalReturn(a->l);
		break;

	// Insert variable's value into correct return value memory space
	case ASSIGNOP:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
		if (a->nodetype == ASSIGNOP)
			evalAssignOp(a);
		else
			evalIncOp(a);

	case VAR_ACCESS:
	case ARR_ACCESS:
		if (a->nodetype == VAR_ACCESS || a->l->nodetype == VAR_ACCESS)
		{
			if (a->nodetype == VAR_ACCESS)
				vnode = ((struct var_node *)a)->val;
			else // a->l->nodetype == VAR_ACCESS
				vnode = ((struct var_node *)a->l)->val;

			v = vnode->val;

			if (v->isGlobal)
				VMQ_addr_prefix = "";
			else if (v->isParam)
				VMQ_addr_prefix = "@/";
			else
				VMQ_addr_prefix = "/-";

			r_val_addr = vnode->VMQ_loc;
		}
		else // a->nodetype == ARR_ACCESS || a->l->nodetype == ARR_ACCESS
		{
			if (a->nodetype == ARR_ACCESS)
				vnode = ((struct var_node *)a->l)->val;
			else
				vnode = ((struct var_node *)a->l->l)->val;

			v = vnode->val;

			VMQ_addr_prefix = "@/-";
			r_val_addr = result->VMQ_loc;
		}

		if (v->var_type == INT && return_type == INT)
			op_code = 'i';
		else if (v->var_type == FLOAT && return_type == FLOAT)
			op_code = 'I';
		else if (v->var_type == FLOAT && return_type == INT)
			op_code = 'f';
		else // v->var_type == INT && return_type == FLOAT
			op_code = 'F';

		VMQ_line = malloc(32);

		sprintf(VMQ_line, "%c %s%d @/4", op_code, VMQ_addr_prefix, r_val_addr);
		appendToVMQList(VMQ_line);

		free(VMQ_line);

		break;

	// Insert result of math op into correct return value memory space.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
		evalMath(a);

		if (result->type == INT && return_type == INT)
			op_code = 'i';
		else if (result->type == FLOAT && return_type == FLOAT)
			op_code = 'I';
		else if (result->type == FLOAT && return_type == INT)
			op_code = 'f';
		else // result->type == INT && return_type == FLOAT
			op_code = 'F';

		VMQ_line = malloc(32);
		sprintf(VMQ_line, "%c /-%d @/4", op_code, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		free(VMQ_line);

		break;

	// Insert return value of function call into return value memory space of current function.
	case FUNC_CALL:
		func = ((struct func_node *)a->l)->val;
		evalFuncCall(a, func->return_type, func->param_list_tail);

		VMQ_line = malloc(32);

		if (return_type == func->return_type)
		{
			sprintf(VMQ_line, "c @/4 %d", func->VMQ_data.quad_start_line);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			appendToVMQList(VMQ_line);
		}
		else
		{
			temp_addr = getNewTempVar(func->return_type);
			sprintf(VMQ_line, "c /-%d %d", temp_addr, func->VMQ_data.quad_start_line);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			appendToVMQList(VMQ_line);

			if (return_type == INT)
				op_code = 'f';
			else // return_type == FLOAT
				op_code = 'F';

			sprintf(VMQ_line, "%c /-%d @/4", op_code, temp_addr);
			appendToVMQList(VMQ_line);

			freeTempVar();
		}

		free(VMQ_line);

		break;

	// Insert address of literal integer or float into return value memory space.
	case INT_LITERAL:
	case FLT_LITERAL:
		if (a->nodetype == INT_LITERAL && return_type == INT)
			op_code = 'i';
		else if (a->nodetype == FLT_LITERAL && return_type == FLOAT)
			op_code = 'I';
		else if (a->nodetype == FLT_LITERAL && return_type == INT)
			op_code = 'f';
		else // a->nodetype == INT_LITERAL && return_type == FLOAT
			op_code = 'F';

		VMQ_line = malloc(32);

		if (a->nodetype == INT_LITERAL)
			sprintf(VMQ_line, "%c %d @/4", op_code, ((struct int_node *)a)->val->VMQ_loc);
		else // a->nodetype == FLT_LITERAL
			sprintf(VMQ_line, "%c %d @/4", op_code, ((struct flt_node *)a)->val->VMQ_loc);

		appendToVMQList(VMQ_line);
		free(VMQ_line);

		break;
	}
}
