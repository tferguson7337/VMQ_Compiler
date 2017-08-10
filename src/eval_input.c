#include "eval.h"

void evalInput(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	// Supporting data structures.
	char *addr_mode = NULL;
	struct var *v = NULL;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;

	unsigned int orig_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;

	switch (a->nodetype)
	{
	// Traverse the subtree.
	case INPUT:
		evalInput(a->l);
		break;

	// Get input for the variable located at the node to the right.
	case STREAMIN:
		evalInput(a->l);

		if (a->r->nodetype == VAR_ACCESS)
		{
			v = ((struct var_node *)a->r)->val;
			if (v->isGlobal)
				addr_mode = "#";
			else if (v->isParam)
				addr_mode = "/";
			else
				addr_mode = "#/-";

			sprintf(VMQ_line, "p %s%d", addr_mode, v->VMQ_loc);
			appendToVMQList(VMQ_line);
		}
		else // a->r->nodetype == ARR_ACCESS
		{
			evalArrAccess(a->r);
			v = ((struct var_node *)a->r->l)->val;

			sprintf(VMQ_line, "p /-%d", result->VMQ_loc);
			appendToVMQList(VMQ_line);
		}

		// Call correct input function (-1 for INT, -2 for FLOAT)
		if (v->var_type == INT)
			appendToVMQList("c 0 -1");
		else
			appendToVMQList("c 0 -2");

		appendToVMQList("^ 2");
		break;
	}

	while (CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
		freeTempVar();
}
