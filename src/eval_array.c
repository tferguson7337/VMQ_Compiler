#include "eval.h"

void evalArrAccess(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		yyerror("evalArrAccess() encountered NULL or invalid \"a\"  nodetype");
	if (!a->r || a->r->nodetype == 0)
		yyerror("evalArrAccess() encountered NULL or invalid \"a->\" nodetype");

	char *addr_mode_l = NULL, *addr_mode_r = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	struct var *l_val = ((struct var_node *)a->l)->val, *r_val = NULL;
	unsigned int lit_loc;

	// Size of VMQ int data type is stored at global memory addr 4
	// Size of VMQ float data type is stored at global memory addr 6
	char type_size_addr = (l_val->var_type == INT) ? '4' : '6';

	if (l_val->isGlobal)
		addr_mode_l = "#";
	else if (l_val->isParam)
		addr_mode_l = "/";
	else
		addr_mode_l = "#/-";

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

		if (expr->l->nodetype == VAR_ACCESS)
		{
			r_val = ((struct var_node *)expr->l)->val;

			if (r_val->var_type == FLOAT)
				yyerror("Floating point value used for array index");

			if (r_val->isGlobal)
				addr_mode_r = "";
			else if (r_val->isParam)
				addr_mode_r = "@/";
			else
				addr_mode_r = "/-";

			result->VMQ_loc = getNewTempVar(ADDR);

			// Generates stmt that will calculate the offset from the array base addr, stored in temp_var
			sprintf(VMQ_line, "m %s%d %c /-%d", addr_mode_r, r_val->VMQ_loc, type_size_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
			// Stores the final address of the l_val array element address, storing it temp_var
			sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		}
		else // expr->l->nodetype == ARR_ACCESS
		{
			r_val = ((struct var_node *)expr->l->l)->val;

			if (r_val->var_type == FLOAT)
				yyerror("Floating point value used for array index");

			result->VMQ_loc = getNewTempVar(ADDR);

			// The first operand (@/-%d) == retrieving the value from the address of r_val's array element
			// that was calculated as a part of evalAssignOp() or evalIncOp().
			sprintf(VMQ_line, "m @/-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);

			sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		}
		 
		freeTempVar();

		break;

	// Evaluate math op, use result to calculate index.
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
		evalMath(expr);
		if (result->type == FLOAT)
			yyerror("evalArrAccess() - Floating point value used for array index");		 

		// Generates stmt that will calculate the offset from the array base addr, store in temp_var.
		sprintf(VMQ_line, "m /-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		// Generates final stmt for calculating array element addr, store in temp_var.
		sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		break;

	case VAR_ACCESS:
		r_val = ((struct var_node *)expr)->val;

		if (r_val->var_type == FLOAT)
			yyerror("Floating point value used for array index");

		if (r_val->isGlobal)
			addr_mode_r = "";
		else if (r_val->isParam)
			addr_mode_r = "@/";
		else
			addr_mode_r = "/-";

		result->VMQ_loc = getNewTempVar(ADDR);

		// Generates stmt that will calculate the offset from the array base addr, stored in temp_var
		sprintf(VMQ_line, "m %s%d %c /-%d", addr_mode_r, r_val->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		// Stores the final address of the l_val array element address, storing it temp_var
		sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		freeTempVar();

		break;

	case FUNC_CALL:
		func = ((struct func_node *)expr->l)->val;

		if (func->return_type == FLOAT)
			yyerror("Floating point value used for array index");

		// Push parameters onto the stack
		evalFuncCall(expr, func->param_list_tail);

		result->VMQ_loc = getNewTempVar(ADDR);

		// The index will be the result of the function call.
		sprintf(VMQ_line, "c #/-%d %d", result->VMQ_loc, func->VMQ_data.quad_start_line);
		appendToVMQList(VMQ_line);

		sprintf(VMQ_line, "m /-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);		 

		break;

	case INT_LITERAL:
	case FLT_LITERAL:
		if (ntype == INT_LITERAL)
			lit_loc = ((struct int_node *)expr)->val->VMQ_loc;
		else
			yyerror("Floating point value used for array index");

		result->VMQ_loc = getNewTempVar(ADDR);

		sprintf(VMQ_line, "m %d %c /-%d", lit_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		freeTempVar();

		break;

	case ARR_ACCESS:
		evalArrAccess(expr);

		sprintf(VMQ_line, "m @/-%d %c /-%d", result->VMQ_loc, type_size_addr, result->VMQ_loc);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "a %s%d /-%d /-%d", addr_mode_l, l_val->VMQ_loc, result->VMQ_loc, result->VMQ_loc);
		appendToVMQList(VMQ_line);

		break;
	}
}
