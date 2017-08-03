#include "eval.h"

void evalInput(struct AST_node *a)
{
	if (!a || a->nodetype == 0)
		return;

	// Supporting data structures.
	char *VMQ_line = NULL, *VMQ_addr_prefix = NULL;
	struct var *v = NULL;
	struct varref *vnode = NULL;
	unsigned int result_type = 0;
	struct VMQ_temp_node *result = &CURRENT_FUNC->VMQ_data.math_result;

	unsigned int orig_addr = CURRENT_FUNC->VMQ_data.tempvar_cur_addr;
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

		VMQ_line = malloc(32);
		if (a->r->nodetype == VAR_ACCESS)
		{
			vnode = ((struct var_node *)a->r)->val;
			v = vnode->val;
			if (v->isGlobal)
				VMQ_addr_prefix = "#";
			else if (v->isParam)
				VMQ_addr_prefix = "/";
			else
				VMQ_addr_prefix = "#/-";

			sprintf(VMQ_line, "p %s%d", VMQ_addr_prefix, vnode->VMQ_loc);
			appendToVMQList(VMQ_line);
		}
		else // a->r->nodetype == ARR_ACCESS
		{
			evalArrAccess(a->r);
			vnode = ((struct var_node *)a->r->l)->val;
			v = vnode->val;

			sprintf(VMQ_line, "p /-%d", result->VMQ_loc);
			appendToVMQList(VMQ_line);
		}

		free(VMQ_line);

		// Call correct input function (-1 for INT, -2 for FLOAT)
		if (v->var_type == INT)
			appendToVMQList("c 0 -1");
		else
			appendToVMQList("c 0 -2");
		break;
	}

	while (CURRENT_FUNC->VMQ_data.tempvar_cur_size != orig_size)
		freeTempVar();
}
