#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data_lists.h"
#include "eval.h"

void eval(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	struct func_list_node *func = NULL;

	switch (a->nodetype)
	{
	// Higher level statement cases.
	case RETURN:
		evalReturn(a);
		break;
	case IF:
	case WHILE:
		evalControl(a);
		break;
	case INPUT:
		evalInput(a);
		break;
	case OUTPUT:
		evalOutput(a);
		break;

	// ASSIGNOP
	case ASSIGNOP:
		evalAssignOp(a);
		break;

	// INCOPs
	case ADD_ASSIGN:
	case SUB_ASSIGN:
		evalIncOp(a);
		break;

	// ADDOPs & MULOPs
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
		evalMath(a);
		break;

	case FUNC_CALL:
		func = ((struct func_node *)a->l)->val;
		evalFuncCall(a, func->param_list_tail);
		sprintf(VMQ_line, "c #/4 %d", func->VMQ_data.quad_start_line);
		appendToVMQList(VMQ_line);
		sprintf(VMQ_line, "^ %d", func->param_count * VMQ_ADDR_SIZE);
		appendToVMQList(VMQ_line);
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
	case ARR_ACCESS:
		return;

	// CURRENT_FUNC pointer should change when encountering a function header (part of FUNC_DEF).
	case FUNC_DEF:
		if (!CURRENT_FUNC)
		{
			CURRENT_FUNC = FUNC_LIST_HEAD;
			CURRENT_FUNC->VMQ_data.quad_start_line = 1;
			CURRENT_FUNC->VMQ_data.quad_end_line = 0; // All functions are guaranteed to have 2 ops
		}
		else
		{
			struct func_list_node *prev = CURRENT_FUNC;
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
		if (CURRENT_FUNC->var_total_size)
			CURRENT_FUNC->var_total_size += 2;

		// If the function ended up using any temporary variables, add two bytes for padding.
		if (CURRENT_FUNC->VMQ_data.tempvar_max_size)
			CURRENT_FUNC->VMQ_data.tempvar_max_size += 2;

		unsigned int total_size = CURRENT_FUNC->var_total_size + CURRENT_FUNC->VMQ_data.tempvar_max_size;

		sprintf(CURRENT_FUNC->VMQ_data.stmt_list_head->VMQ_line, "# %d", total_size);

		if (strcmp(CURRENT_FUNC->func_name, "main") == 0)
			appendToVMQList("h"); // Program ends after returning from main().
		else
			appendToVMQList("/"); // Op for returning to caller function().

		break;

	case PROG:
		CURRENT_FUNC = NULL;
		eval(a->r);
		break;

	// All other non-terminal cases are traversed.
	default:
		eval(a->l);
		eval(a->r);
	}
}
