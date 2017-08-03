#include "eval.h"

void evalAssignOp(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;
	if (!a->r || a->r->nodetype == 0)
		return;

	// Supporting data structures, shortens code in various switch cases.
	char *VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
	char op_code;
	struct varref *l_node;
	struct var *l_val;
	struct varref *r_node = NULL;
	struct var *r_val = NULL;
	struct intlit *r_int = NULL;
	struct fltlit *r_flt = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;

	int ntype = a->r->nodetype;

	unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
	unsigned int lhs_addr, rhs_addr;

	// Get the VMQ location values and pointers to the right data structures set up for the LHS of ASSIGN_OP
	if (a->l->nodetype == VAR_ACCESS)
	{
		l_node = ((struct var_node *)a->l)->val;
		l_val = l_node->val;
		lhs_addr = l_node->VMQ_loc;

		if (l_val->isGlobal)
			VMQ_addr_l_prefix = "";
		else if (l_val->isParam)
			VMQ_addr_l_prefix = "@/";
		else
			VMQ_addr_l_prefix = "/-";
	}
	else // a->l->nodetype == ARR_ACCESS
	{
		l_node = ((struct var_node *)a->l->l)->val;
		l_val = l_node->val;
		// There are some cases where lhs_addr must be done *after* rhs_addr,
		// so it is handled in each case.

		VMQ_addr_l_prefix = "@/-";
	}

	// Left child of (a) is guaranteed to be a variable.
	// Right child is either an ASSIGNOP, INCOP, ADDOP, MULOP, INT/FLT_LITERAL, VAR_ACCESS, FUNC_CALL, ARR_ACCESS
	switch (ntype)
	{
	// Place value of right child into left child (variable).
	case ASSIGNOP:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
		if (ntype == ASSIGNOP)
			evalAssignOp(a->r);
		else
			evalIncOp(a->r);

		if (a->r->l->nodetype == VAR_ACCESS)
		{
			r_node = ((struct var_node *)a->r->l)->val;
			r_val = r_node->val;

			rhs_addr = r_node->VMQ_loc;

			if (r_val->isGlobal)
				VMQ_addr_r_prefix = "";
			else if (r_val->isParam)
				VMQ_addr_r_prefix = "@/";
			else
				VMQ_addr_r_prefix = "/-";
		}
		else // a->r->l->nodetype == ARR_ACCESS
		{
			r_node = ((struct var_node *)a->r->l->l)->val;
			r_val = r_node->val;

			rhs_addr = getNewTempVar(ADDR);
			VMQ_addr_r_prefix = "@/-";
		}
		r_val = r_node->val;

		if (l_val->var_type == INT && r_val->var_type == INT)
			op_code = 'i';
		else if (l_val->var_type == FLOAT && r_val->var_type == FLOAT)
			op_code = 'I';
		else if (l_val->var_type == INT && r_val->var_type == FLOAT)
			op_code = 'f';
		else
			op_code = 'F';

		if (a->l->nodetype == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = result->VMQ_loc;
		}

		VMQ_line = malloc(32);
		sprintf(VMQ_line, "%c %s%d %s%d", op_code, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);
		free(VMQ_line);

		if (a->r->l->nodetype == ARR_ACCESS)
			freeTempVar();

		break;

	// Place value of literal into variable.
	case INT_LITERAL:
	case FLT_LITERAL:
		if (ntype == INT_LITERAL)
		{
			rhs_addr = ((struct int_node *)a->r)->val->VMQ_loc;
			if (l_val->var_type == INT)
				op_code = 'i';
			else
				op_code = 'F';
		}
		else
		{
			rhs_addr = ((struct flt_node *)a->r)->val->VMQ_loc;
			if (l_val->var_type == INT)
				op_code = 'f';
			else
				op_code = 'I';
		}

		if (a->l->nodetype == ARR_ACCESS)
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
	case VAR_ACCESS:
		r_node = ((struct var_node *)a->r)->val;
		r_val = r_node->val;
		rhs_addr = r_node->VMQ_loc;
		if (l_val->var_type == r_val->var_type)
		{
			if (l_val->var_type == INT_LITERAL)
				op_code = 'i';
			else
				op_code = 'I';
		}
		else
		{
			if (l_val->var_type == INT_LITERAL)
				op_code = 'f';
			else
				op_code = 'F';
		}

		if (r_val->isGlobal)
			VMQ_addr_r_prefix = "";
		else if (r_val->isParam)
			VMQ_addr_r_prefix = "@/";
		else
			VMQ_addr_r_prefix = "/-";

		if (a->l->nodetype == ARR_ACCESS)
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
	case MOD:
		if (a->l->nodetype == ARR_ACCESS)
		{
			evalArrAccess(a->l);

			// We call getNewTempVar here to keep the result of the math op
			// below from overwritting the address stored from the ARR_ACCESS
			// evaluation just above.
			lhs_addr = getNewTempVar(ADDR);
		}

		if (ntype == ADD || ntype == SUB)
			evalAddOp(a->r);
		else
			evalMulOp(a->r);

		rhs_addr = result->VMQ_loc;

		if (result->type == INT)
		{
			if (l_val->var_type == INT)
				op_code = 'i';
			else
				op_code = 'F';
		}
		else // result->type == FLOAT
		{
			if (l_val->var_type == FLOAT)
				op_code = 'I';
			else
				op_code = 'f';
		}

		VMQ_line = malloc(32);
		sprintf(VMQ_line, "%c /-%d %s%d", op_code, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);
		free(VMQ_line);

		if (a->l->nodetype == ARR_ACCESS)
			freeTempVar();

		break;

	// Place value of array element into l_val variable.
	case ARR_ACCESS:
		evalArrAccess(a->l);
		lhs_addr = getNewTempVar(ADDR);
		evalArrAccess(a->r);
		rhs_addr = getNewTempVar(ADDR);
		// evalArrAccess will write the commands that store the address of the array element in the first temporary variable.

		r_node = ((struct var_node *)a->r->l)->val;
		r_val = r_node->val;

		if (l_val->var_type == r_val->var_type)
		{
			if (l_val->var_type == INT)
				op_code = 'i';
			else
				op_code = 'I';
		}
		else
		{
			if (l_val->var_type == INT)
				op_code = 'f';
			else
				op_code = 'F';
		}

		VMQ_line = malloc(32);
		sprintf(VMQ_line, "%c @/-%d %s%d", op_code, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);
		free(VMQ_line);

		freeTempVar();
		freeTempVar();

		break;

	// Place return value of function into l_val variable.
	case FUNC_CALL:
		if (a->l->nodetype == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
			VMQ_addr_l_prefix = "/-";
		}
		else // Function semantics are a bit different, so adjust memory access mode as needed.
		{
			if (l_val->isGlobal)
				VMQ_addr_l_prefix = "#";
			else if (l_val->isParam)
				VMQ_addr_l_prefix = "/";
			else
				VMQ_addr_l_prefix = "#/-";
		}

		// Generates the appropriate statements for pushing variables onto the stack for passing.
		func = ((struct func_node *)a->r->l)->val;
		evalFuncCall(a->r, func->return_type, func->param_list_tail);

		if (l_val->var_type == func->return_type)
		{ // We can store the return value of the function directly into the l_val
			VMQ_line = malloc(32);
			sprintf(VMQ_line, "c %s%d %d", VMQ_addr_l_prefix, lhs_addr, func->VMQ_data.quad_start_line);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			appendToVMQList(VMQ_line);
			free(VMQ_line);
		}
		else
		{ // We have to store into a temporary variable and then cast the result into the l_val
			if (l_val->var_type == INT)
				op_code = 'f';
			else
				op_code = 'F';

			unsigned int temp_addr;

			if (func->return_type == INT)
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

		if (a->l->nodetype == ARR_ACCESS)
			freeTempVar();

		break;
	}

	while (CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
		freeTempVar();
}