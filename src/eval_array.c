#include "eval.h"

void evalArrAccess(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		yyerror("evalArrAccess() encountered NULL or invalid \"a\"  nodetype");
	if (!a->r || a->r->nodetype == 0)
		yyerror("evalArrAccess() encountered NULL or invalid \"a->\" nodetype");

	char *VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	struct varref *l_node = ((struct var_node *)a->l)->val, *r_node = NULL;
	struct var *l_val = l_node->val, *r_val = NULL;
	unsigned int lit_loc;

	// Size of VMQ int data type is stored at global memory addr 4
	// Size of VMQ float data type is stored at global memory addr 6
	char type_size_addr = (l_val->var_type == INT) ? '4' : '6';

	if (l_val->isGlobal)
		VMQ_addr_l_prefix = "#";
	else if (l_val->isParam)
		VMQ_addr_l_prefix = "/";
	else
		VMQ_addr_l_prefix = "#/-";

	// expr must be evaluated to determine array index
	struct AST_node *expr = a->r;

	unsigned int ntype = expr->nodetype;
	switch (ntype)
	{
	// Evaluate assignment-based op, use result to calculate index.
	// r_val is either a variable or an array element
	case ASSIGNOP:
	case INCOP:
		if (ntype == ASSIGNOP)
			evalAssignOp(expr);
		else
			evalIncOp(expr);

		VMQ_line = malloc(32);

		if (expr->l->nodetype == VAR_ACCESS)
		{
			r_node = ((struct var_node *)expr->l)->val;
			r_val = r_node->val;

			if (r_val->var_type == FLOAT)
				yyerror("Floating point value used for array index");

			if (r_val->isGlobal)
				VMQ_addr_r_prefix = "";
			else if (r_val->isParam)
				VMQ_addr_r_prefix = "@/";
			else
				VMQ_addr_r_prefix = "/-";

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
			r_node = ((struct var_node *)expr->l->l)->val;
			r_val = r_node->val;

			if (r_val->var_type == FLOAT)
				yyerror("Floating point value used for array index");

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
	case MOD:
		if (ntype == ADD || ntype == SUB)
			evalAddOp(expr);
		else
			evalMulOp(expr);

		if (result->type == FLOAT)
			yyerror("evalArrAccess() - Floating point value used for array index");

		VMQ_line = malloc(32);

		// Generates stmt that will calculate the offset from the array base addr, store in temp_var.
		sprintf(VMQ_line, "m /-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		// Generates final stmt for calculating array element addr, store in temp_var.
		sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		break;

	case VAR_ACCESS:
		r_node = ((struct var_node *)expr)->val;
		r_val = r_node->val;

		if (r_val->var_type == FLOAT)
			yyerror("Floating point value used for array index");

		if (r_val->isGlobal)
			VMQ_addr_r_prefix = "";
		else if (r_val->isParam)
			VMQ_addr_r_prefix = "@/";
		else
			VMQ_addr_r_prefix = "/-";

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

	case FUNC_CALL:
		func = ((struct func_node *)expr->l)->val;

		if (func->return_type == FLOAT)
			yyerror("Floating point value used for array index");

		// Push parameters onto the stack
		evalFuncCall(expr, func->return_type, func->param_list_tail);

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
	case FLT_LITERAL:
		if (ntype == INT_LITERAL)
			lit_loc = ((struct int_node *)expr)->val->VMQ_loc;
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

	case ARR_ACCESS:
		evalArrAccess(expr);

		VMQ_line = malloc(32);

		sprintf(VMQ_line, "m @/-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "a %s%d /-%d /-%d", VMQ_addr_l_prefix, l_node->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		free(VMQ_line);

		break;
	}
}