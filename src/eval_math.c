#include "eval.h"

void evalMath(struct AST_node *a)
{
    if (!a || a->nodetype == 0)
		yyerror("evalAddOp() encountered NULL or empty AST node");

	// Supporting data structures
	char *VMQ_line = NULL, *VMQ_addr_l_prefix = NULL, *VMQ_addr_r_prefix = NULL;
	char op_code;
	struct varref *l_node = NULL, *r_node = NULL;
	struct var *l_val = NULL, *r_val = NULL;
	struct intlit *r_int = NULL;
	struct fltlit *r_flt = NULL;
	struct func_list_node *func = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;
	unsigned int result_type = INT;
	unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
	unsigned int lhs_addr, rhs_addr, new_addr, dest_addr;

	struct AST_node *lhs = a->l, *rhs = a->r;
	unsigned int lhs_ntype = lhs->nodetype, rhs_ntype;

	// For unary math ops, rhs will be NULL.
	if (rhs)
		rhs_ntype = rhs->nodetype;
	else
		rhs_ntype = 0;

	// Used to track if the lhs/rhs is an INT or FLOAT,
	// so we can cast as needed while evaluating ADDOPs.
	unsigned int lhs_datatype, rhs_datatype;

	unsigned int ntype = a->nodetype;

    switch (ntype)
    {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
	    
	    VMQ_line = malloc(32);
	    if (isMathLeaf(lhs_ntype) || lhs_ntype == FUNC_CALL)
	    {
		if (lhs_ntype == INT_LITERAL)
		{
		    lhs_datatype = INT;
		    VMQ_addr_l_prefix = "";
		    lhs_addr = ((struct int_node *)lhs)->val->VMQ_loc;
		}
		else if (lhs_ntype == FLT_LITERAL)
		{
		    lhs_datatype = FLOAT;
		    VMQ_addr_l_prefix = "";
		    lhs_addr = ((struct flt_node *)lhs)->val->VMQ_loc;
		}
			else if (lhs_ntype == VAR_ACCESS)
			{
				l_node = ((struct var_node *)lhs)->val;
				l_val = l_node->val;

				lhs_datatype = l_val->var_type;

				if (l_val->isGlobal)
					VMQ_addr_l_prefix = "";
				else if (l_val->isParam)
					VMQ_addr_l_prefix = "@/";
				else
					VMQ_addr_l_prefix = "/-";

				lhs_addr = l_node->VMQ_loc;
			}
			else if (lhs_ntype == ARR_ACCESS)
			{
				lhs_datatype = ((struct var_node *)lhs->l)->val->val->var_type;
				VMQ_addr_l_prefix = "@/-";
				evalArrAccess(lhs);
				lhs_addr = getNewTempVar(ADDR);
			}
			else if (lhs_ntype == FUNC_CALL)
			{
				func = ((struct func_node *)lhs->l)->val;
				lhs_datatype = func->return_type;
				VMQ_addr_l_prefix = "/-";
				evalFuncCall(lhs, func->return_type, func->param_list_tail);
				lhs_addr = getNewTempVar(lhs_datatype);
				sprintf(VMQ_line, "c #/-%d %d", lhs_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
			}
		}
		else
		{
			evalMath(lhs);

			lhs_datatype = result->type;
			VMQ_addr_l_prefix = "/-";
			lhs_addr = result->VMQ_loc;
		}

		// Special case:  unary add/sub.
		if (rhs_ntype == 0)
		{
			// If lhs is the result of a function call or math op evaluation, then we already allocated a
			// temporary variable for it - no need to allocate another for the unary op.
			result->VMQ_loc = new_addr = (isMathLeaf(lhs_ntype) && lhs_ntype != FUNC_CALL) ? getNewTempVar(lhs_datatype) : lhs_addr;

			if (lhs_datatype == INT)
			{
				if (ntype == ADD)
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
				if (ntype == ADD)
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
		else if (isMathLeaf(rhs_ntype) || rhs_ntype == FUNC_CALL)
		{
			if (rhs_ntype == INT_LITERAL)
			{
				rhs_datatype = INT;
				VMQ_addr_r_prefix = "";
				rhs_addr = ((struct int_node *)rhs)->val->VMQ_loc;
			}
			else if (rhs_ntype == FLT_LITERAL)
			{
				rhs_datatype = FLOAT;
				VMQ_addr_r_prefix = "";
				rhs_addr = ((struct flt_node *)rhs)->val->VMQ_loc;
			}
			else if (rhs_ntype == VAR_ACCESS)
			{
				r_node = ((struct var_node *)rhs)->val;
				r_val = r_node->val;

				rhs_datatype = r_val->var_type;

				if (r_val->isGlobal)
					VMQ_addr_r_prefix = "";
				else if (r_val->isParam)
					VMQ_addr_r_prefix = "@/";
				else
					VMQ_addr_r_prefix = "/-";

				rhs_addr = r_node->VMQ_loc;
			}
			else if (rhs_ntype == ARR_ACCESS)
			{
				rhs_datatype = ((struct var_node *)rhs->l)->val->val->var_type;
				VMQ_addr_r_prefix = "@/-";
				evalArrAccess(rhs);
				rhs_addr = getNewTempVar(ADDR);
			}
			else if (rhs_ntype == FUNC_CALL)
			{
				func = ((struct func_node *)rhs->l)->val;
				rhs_datatype = func->return_type;
				VMQ_addr_r_prefix = "/-";
				evalFuncCall(rhs, func->return_type, func->param_list_tail);
				rhs_addr = getNewTempVar(rhs_datatype);
				sprintf(VMQ_line, "c #/-%d %d", rhs_addr, func->VMQ_data.quad_start_line);
				appendToVMQList(VMQ_line);
			}
		}
		else // rhs is some math op, needs to be evaluated
		{
			evalMath(rhs);
			rhs_datatype = result->type;
			VMQ_addr_r_prefix = "/-";
			rhs_addr = result->VMQ_loc;
		}

		// Casting is required before generating the ADD/SUB operation.
		if (lhs_datatype != rhs_datatype)
		{
			if (ntype == MOD)
				yyerror("evalMulOp() - operand for residue (modulo) op cannot be float value");


			unsigned int *addr = (lhs_datatype == INT) ? &lhs_addr : &rhs_addr;
			unsigned int *ntype_ptr = (lhs_datatype == INT) ? &lhs_ntype : &rhs_ntype;
			unsigned int *datatype = (lhs_datatype == INT) ? &lhs_datatype : &rhs_datatype;
			char **prefix = (lhs_datatype == INT) ? &VMQ_addr_l_prefix : &VMQ_addr_r_prefix;

			unsigned int temp_start_addr = CURRENT_FUNC->VMQ_data.tempvar_start;

			if (isMathLeaf(*ntype_ptr))
				new_addr = getNewTempVar(FLOAT);
			else
			{
				if (*addr == temp_start_addr)
					new_addr = *addr;
				else
					new_addr = getNewTempVar(FLOAT);
			}

			sprintf(VMQ_line, "F %s%d /-%d", *prefix, *addr, new_addr);
			appendToVMQList(VMQ_line);
			*prefix = "/-";
			*addr = new_addr;

			result->type == FLOAT;

			if(ntype == ADD || ntype == SUB)
			    op_code = (ntype == ADD) ? 'A' : 'S';
			else
			    op_code = (ntype == MUL) ? 'M' : 'D';

			if (isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
				result->VMQ_loc = new_addr;
			else if (isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
			{
				result->VMQ_loc = (rhs_datatype == FLOAT) ? rhs_addr : new_addr;
				if (*addr != temp_start_addr)
				{
					freeTempVar();
					freeTempVar();
				}
			}
			else if (!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
			{
				result->VMQ_loc = (lhs_datatype == FLOAT) ? lhs_addr : new_addr;
				if (*addr != temp_start_addr)
				{
					freeTempVar();
					freeTempVar();
				}
			}
			else
			{
				result->VMQ_loc = (lhs_addr < rhs_addr) ? lhs_addr : rhs_addr;
				if (*addr != temp_start_addr)
				{
					freeTempVar(); // Need to cleanup two temps here, one for the
					freeTempVar(); // temp storing the other addr used in the op
								   // and another for the cast temp created
				}
			}
			sprintf(VMQ_line, "%c %s%d %s%d /-%d", op_code, VMQ_addr_l_prefix, lhs_addr, VMQ_addr_r_prefix, rhs_addr, result->VMQ_loc);
			appendToVMQList(VMQ_line);
		}
		else
		{
			if (ntype == ADD)
				op_code = (lhs_datatype == INT) ? 'a' : 'A';
			else if (ntype == SUB)
				op_code = (lhs_datatype == INT) ? 's' : 'S';
			else if (ntype == MUL)
				op_code = (lhs_datatype == INT) ? 'm' : 'M';
			else if (ntype == DIV)
				op_code = (lhs_datatype == INT) ? 'd' : 'D';
			else
			{
				if (lhs_datatype == FLOAT)
					yyerror("evalMulOp() - operand for residue (modulo) op cannot be float value");
				else
					op_code = 'r';
			}

			result->type = lhs_ntype;

			if (isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
				result->VMQ_loc = getNewTempVar(lhs_datatype);
			else if (isMathLeaf(lhs_ntype) && !isMathLeaf(rhs_ntype))
				result->VMQ_loc = rhs_addr;
			else if (!isMathLeaf(lhs_ntype) && isMathLeaf(rhs_ntype))
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

	if (lhs_datatype == FLOAT || rhs_datatype == FLOAT)
		result->type = FLOAT;
	else
		result->type = INT;
}
