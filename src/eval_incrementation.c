#include "eval.h"

void evalIncOp(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;
	if (!a->r || a->r->nodetype == 0)
		return;

	// Supporting data structures, shortens code in various switch cases.
	char op_code;
	char *VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
	struct func_list_node *func = NULL;
	struct varref *l_node = NULL, *r_node = NULL;
	struct var *l_val = NULL, *r_val = NULL;
	struct intlit *i_lit = NULL;
	struct fltlit *f_lit = NULL;

	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	unsigned int lhs_type, lhs_addr, rhs_addr;
	unsigned int temp_addr = 0;

	unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

	// Initial setup for LHS data (LHS is a VAR_ACCESS or ARR_ACCESS)
	if (a->l->nodetype == VAR_ACCESS)
	{
		l_node = ((struct var_node *)a->l)->val;
		l_val = l_node->val;

		if (l_val->isGlobal)
			VMQ_addr_l_prefix = "";
		else if (l_val->isParam)
			VMQ_addr_l_prefix = "@/";
		else
			VMQ_addr_l_prefix = "/-";

		lhs_type = VAR_ACCESS;
		lhs_addr = l_node->VMQ_loc;
	}
	else // a->l->nodetype == ARR_ACCESS
	{
		l_node = ((struct var_node *)a->l->l)->val;
		l_val = l_node->val;

		VMQ_addr_l_prefix = "@/-";
		lhs_type = ARR_ACCESS;
	}

	// a->l is guaranteed to be a variable - expr may need to be evaluated.
	struct AST_node *expr = a->r;
	int ntype = expr->nodetype;

	switch (ntype)
	{
	/* 
	    Cases:  ASSIGNOP, ADD_ASSIGN, SUB_ASSIGN, ADD, SUB, MUL, DIV, MOD
		    VAR_ACCESS, FUNC_CALL, INT_LITERAL, FLT_LITERAL, ARR_ACCESS
	*/
	case ASSIGNOP:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
		if (ntype == ASSIGNOP)
			evalAssignOp(expr);
		else
			evalIncOp(expr);

		if (expr->l->nodetype == VAR_ACCESS)
		{
			r_node = ((struct var_node *)expr->l)->val;
			r_val = r_node->val;

			if (r_val->isGlobal)
				VMQ_addr_r_prefix = "";
			else if (r_val->isParam)
				VMQ_addr_r_prefix = "@/";
			else
				VMQ_addr_r_prefix = "/-";

			rhs_addr = r_node->VMQ_loc;
		}
		else // expr->l->nodetype == ARR_ACCESS
		{
			r_node = ((struct var_node *)expr->l->l)->val;
			r_val = r_node->val;

			rhs_addr = getNewTempVar(ADDR);
			VMQ_addr_r_prefix = "@/-";
		}

		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		VMQ_line = malloc(32);
		if (l_val->var_type == r_val->var_type)
		{
			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);
		}
		else // Casting is required.
		{
			if (l_val->var_type == INT)
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

			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);

			freeTempVar();
		}

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		if (expr->l->nodetype == ARR_ACCESS)
			freeTempVar();

		free(VMQ_line);

		break;

	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
		if (ntype == ADD || ntype == SUB)
			evalAddOp(expr);
		else
			evalMulOp(expr);

		rhs_addr = result->VMQ_loc;

		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		VMQ_line = malloc(32);
		if (l_val->var_type == result->type)
		{
			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);
		}
		else
		{
			if (l_val->var_type == INT)
				op_code = 'f';
			else
				op_code = 'F';

			if (op_code == 'F' && (temp_addr % VMQ_FLT_SIZE != 0))
				temp_addr = getNewTempVar(FLOAT);
			else
				temp_addr = rhs_addr;

			sprintf(VMQ_line, "%c /-%d /-%d", op_code, rhs_addr, temp_addr);
			appendToVMQList(VMQ_line);

			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);

			if (temp_addr != rhs_addr)
				freeTempVar();
		}

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		free(VMQ_line);

		break;

	case VAR_ACCESS:
		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		r_node = ((struct var_node *)expr)->val;
		r_val = r_node->val;

		rhs_addr = r_node->VMQ_loc;

		if (r_val->isGlobal)
			VMQ_addr_r_prefix = "";
		else if (r_val->isParam)
			VMQ_addr_r_prefix = "@/";
		else
			VMQ_addr_r_prefix = "/-";

		VMQ_line = malloc(32);
		if (l_val->var_type == r_val->var_type)
		{
			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);
		}
		else
		{
			if (l_val->var_type == INT)
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

			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, temp_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);

			freeTempVar();
		}

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		free(VMQ_line);

		break;

	case FUNC_CALL:
		func = ((struct func_node *)expr)->val;
		evalFuncCall(expr, func->return_type, func->param_list_tail);

		if (func->return_type == INT)
			rhs_addr = temp_addr = getNewTempVar(INT);
		else
			rhs_addr = temp_addr = getNewTempVar(FLOAT);

		VMQ_line = malloc(32);

		sprintf(VMQ_line, "c #/-%d %d", rhs_addr, func->VMQ_data.quad_start_line);
		appendToVMQList(VMQ_line);

		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		if (l_val->var_type == func->return_type)
		{
			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);
		}
		else
		{
			op_code = (l_val->var_type == INT) ? 'f' : 'F';
			if (op_code == 'F' && (temp_addr % VMQ_FLT_SIZE != 0))
				rhs_addr = getNewTempVar(FLOAT);

			sprintf(VMQ_line, "%c /-%d /-%d", op_code, temp_addr, rhs_addr);
			appendToVMQList(VMQ_line);

			if (a->nodetype == ADD_ASSIGN)
				op_code = (l_val->var_type == INT) ? 'a' : 'A';
			else // SUB_ASSIGN INCOP
				op_code = (l_val->var_type == INT) ? 's' : 'S';

			sprintf(VMQ_line, "%c %s%d /-%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
			appendToVMQList(VMQ_line);

			if (rhs_addr != temp_addr)
				freeTempVar();
		}

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		freeTempVar();

		free(VMQ_line);

		break;

	case INT_LITERAL:
		i_lit = ((struct int_node *)expr)->val;
		rhs_addr = i_lit->VMQ_loc;

		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		VMQ_line = malloc(32);
		if (l_val->var_type != INT)
		{
			temp_addr = getNewTempVar(FLOAT);
			sprintf(VMQ_line, "F %d /-%d", rhs_addr, temp_addr);
			appendToVMQList(VMQ_line);
			rhs_addr = temp_addr;
			VMQ_addr_r_prefix = "/-";
		}
		else
			VMQ_addr_r_prefix = "";

		if (a->nodetype == ADD_ASSIGN)
			op_code = (l_val->var_type == INT) ? 'a' : 'A';
		else // SUB_ASSIGN INCOP
			op_code = (l_val->var_type == INT) ? 's' : 'S';

		sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);

		if (rhs_addr != i_lit->VMQ_loc)
			freeTempVar();

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		free(VMQ_line);

		break;

	case FLT_LITERAL:
		f_lit = ((struct flt_node *)expr)->val;
		rhs_addr = f_lit->VMQ_loc;

		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		VMQ_line = malloc(32);
		if (l_val->var_type != FLOAT)
		{
			temp_addr = getNewTempVar(INT);
			sprintf(VMQ_line, "f %d /-%d", rhs_addr, temp_addr);
			appendToVMQList(VMQ_line);
			rhs_addr = temp_addr;
			VMQ_addr_r_prefix = "/-";
		}
		else
			VMQ_addr_r_prefix = "";

		if (a->nodetype == ADD_ASSIGN)
			op_code = (l_val->var_type == INT) ? 'a' : 'A';
		else // SUB_ASSIGN INCOP
			op_code = (l_val->var_type == INT) ? 's' : 'S';

		sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);

		if (rhs_addr != f_lit->VMQ_loc)
			freeTempVar();

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		free(VMQ_line);

		break;

	case ARR_ACCESS:
		if (lhs_type == ARR_ACCESS)
		{
			evalArrAccess(a->l);
			lhs_addr = getNewTempVar(ADDR);
		}

		evalArrAccess(expr);
		rhs_addr = getNewTempVar(ADDR);

		VMQ_addr_r_prefix = "@/-";
		r_node = ((struct var_node *)expr->l)->val;
		r_val = r_node->val;

		VMQ_line = malloc(32);

		if (l_val->var_type != r_val->var_type)
		{
			if (l_val->var_type == INT)
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

		if (a->nodetype == ADD_ASSIGN)
			op_code = (l_val->var_type == INT) ? 'a' : 'A';
		else // SUB_ASSIGN INCOP
			op_code = (l_val->var_type == INT) ? 's' : 'S';

		sprintf(VMQ_line, "%c %s%d %s%d %s%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, VMQ_addr_l_prefix, lhs_addr);
		appendToVMQList(VMQ_line);

		if (temp_addr)
			freeTempVar();

		// RHS array ADDR storage
		freeTempVar();

		if (lhs_type == ARR_ACCESS)
			freeTempVar();

		break;
	}

	while (CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
		freeTempVar();
}