#include "eval.h"
#include "conditional_helper_functions.h"

void evalIf(struct AST_node *a)
{
	struct AST_node *cond_code = ((struct ctrl_node *)a)->c;
	struct AST_node *true_code = ((struct ctrl_node *)a)->t;
	struct AST_node *false_code = ((struct ctrl_node *)a)->f;

	struct func_list_node *func = CURRENT_FUNC;

	unsigned int not_flag = 0;

	if (cond_code->nodetype == NOT)
	{
		// If the cond_code tree starts with 1 or more NOTs,
		// delete them while setting the not_flag.
		struct AST_node *del_node = NULL;
		while (cond_code->nodetype == NOT)
		{
			del_node = cond_code;
			cond_code = cond_code->l;
			free(del_node);
			not_flag = !not_flag; // Two NOTs cancel each other out.
		}

		((struct ctrl_node *)a)->c = cond_code;
	}

	if (isRelOp(cond_code->nodetype) && not_flag)
		cond_code->nodetype = getRelOpComplement(cond_code->nodetype);
	else if (!isRelOp(cond_code->nodetype))
		DMTransformTree(&cond_code, not_flag);

	struct logic_node *temp_cond = NULL;
	if (!isRelOp(cond_code->nodetype))
		configureLogicNodes(&cond_code);
	else
	{ // Special case:  Conditional code is a simple relational operation.
		// This simplifies the process later on when we need to fill in the dummy jump statements.
		temp_cond = (struct logic_node*)create_logic_node(0, cond_code, NULL);
		appendToCondList(temp_cond);
	}

	// Evaluates the LHS and RHS of relational operators, generates incomplete un/conditional jump statements
	// that are handled later on and are accessible from the global COND_LIST_HEAD pointer.
	evalCond();

	// Generate false (else) code.
	unsigned int false_line_start = func->VMQ_data.quad_end_line + 1;
	struct VMQ_list_node *false_jump_line = NULL;
	eval(false_code);

	// Insert dummy string for end-of-false-code jump statement, save the line for later.
	appendToVMQList("");
	false_jump_line = func->VMQ_data.stmt_list_tail;

	// Generate true (if) code.
	unsigned int true_line_start = func->VMQ_data.quad_end_line + 1;
	eval(true_code);

	// Update the jump string for the end-of-false-code block
	sprintf(false_jump_line->VMQ_line, "j %d", func->VMQ_data.quad_end_line + 1);

	// Set all of the un/conditional jump statements
	setJumpStatements(true_line_start, false_line_start);
}
