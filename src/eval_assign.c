#include "eval.h"

void evalAssignOp(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;
	if (!a->r || a->r->nodetype == 0)
		return;

	// Supporting data structures, shortens code in various switch cases.
	char *addr_mode_l = NULL, *addr_mode_r = NULL;
	char op_code;
	struct var *l_val = NULL, *r_val = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;

	int ntype = a->r->nodetype;

	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
	unsigned int lhs_addr, rhs_addr;

	// Get the VMQ location values and pointers to the right data structures set up for the LHS of ASSIGN_OP
	if (a->l->nodetype == VAR_ACCESS)
	{
		l_val = ((struct var_node *)a->l)->val;
		lhs_addr = l_val->VMQ_loc;

		if (l_val->isGlobal)
			addr_mode_l = "";
		else if (l_val->isParam)
			addr_mode_l = "@/";
		else
			addr_mode_l = "/-";
	}
	else // a->l->nodetype == ARR_ACCESS
	{
		l_val = ((struct var_node *)a->l->l)->val;
		// There are some cases where lhs_addr must be done *after* rhs_addr,
		// so it is handled in each case.

		addr_mode_l = "@/-";
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
			r_val = ((struct var_node *)a->r->l)->val;

			rhs_addr = r_val->VMQ_loc;

			if (r_val->isGlobal)
				addr_mode_r = "";
			else if (r_val->isParam)
				addr_mode_r = "@/";
			else
				addr_mode_r = "/-";
		}
		else // a->r->l->nodetype == ARR_ACCESS
		{
			r_val = ((struct var_node *)a->r->l->l)->val;

			rhs_addr = getNewTempVar(ADDR);
			addr_mode_r = "@/-";
		}

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
		 
		sprintf(VMQ_line, "%c %s%d %s%d", op_code, addr_mode_r, rhs_addr, addr_mode_l, lhs_addr);
		appendToVMQList(VMQ_line);		 

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

		sprintf(VMQ_line, "%c %d %s%d", op_code, rhs_addr, addr_mode_l, lhs_addr);
		appendToVMQList(VMQ_line);

		break;

	// Place value of r_val variable into l_val variable.
	case VAR_ACCESS:
		r_val = ((struct var_node *)a->r)->val;
		rhs_addr = r_val->VMQ_loc;
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
			addr_mode_r = "";
		else if (r_val->isParam)
			addr_mode_r = "@/";
		else
			addr_mode_r = "/-";

		if (a->l->nodetype == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = result->VMQ_loc;
		}

		sprintf(VMQ_line, "%c %s%d %s%d", op_code, addr_mode_r, rhs_addr, addr_mode_l, lhs_addr);
		appendToVMQList(VMQ_line);		 

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

		evalMath(a->r);

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
		 
		sprintf(VMQ_line, "%c /-%d %s%d", op_code, rhs_addr, addr_mode_l, lhs_addr);
		appendToVMQList(VMQ_line); 

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

		r_val = ((struct var_node *)a->r->l)->val;

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
		
		sprintf(VMQ_line, "%c @/-%d %s%d", op_code, rhs_addr, addr_mode_l, lhs_addr);
		appendToVMQList(VMQ_line);
	
		freeTempVar();
		freeTempVar();

		break;

	// Place return value of function into l_val variable.
	case FUNC_CALL:
		if (a->l->nodetype == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
			addr_mode_l = "/-";
		}
		else // Function semantics are a bit different, so adjust memory access mode as needed.
		{
			if (l_val->isGlobal)
				addr_mode_l = "#";
			else if (l_val->isParam)
				addr_mode_l = "/";
			else
				addr_mode_l = "#/-";
		}

		// Generates the appropriate statements for pushing variables onto the stack for passing.
		func = ((struct func_node *)a->r->l)->val;
		evalFuncCall(a->r, func->param_list_tail);

		if (l_val->var_type == func->return_type)
		{ // We can store the return value of the function directly into the l_val
			 
			sprintf(VMQ_line, "c %s%d %d", addr_mode_l, lhs_addr, func->VMQ_data.quad_start_line);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			appendToVMQList(VMQ_line);		 
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
			 
			sprintf(VMQ_line, "c #/-%d %d", temp_addr, func->VMQ_data.quad_start_line);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
			appendToVMQList(VMQ_line);
			sprintf(VMQ_line, "%c /-%d %s%d", op_code, temp_addr, addr_mode_l, lhs_addr);
			appendToVMQList(VMQ_line);
	
			freeTempVar();
		}

		if (a->l->nodetype == ARR_ACCESS)
			freeTempVar();

		break;
	}

	while (CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
		freeTempVar();
}
