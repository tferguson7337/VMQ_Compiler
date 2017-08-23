#include "eval.h"
#include "conditional_helper_functions.h"

void evalControl(struct AST_node *a)
{
	struct AST_node *cond_code = ((struct ctrl_node *)a)->c;
	struct AST_node *true_code = ((struct ctrl_node *)a)->t;
	struct AST_node *false_code = ((struct ctrl_node *)a)->f;

	struct cond_list c_list;

	c_list.head = c_list.tail = NULL;

	struct func_list_node *func = CURRENT_FUNC;

	unsigned int not_flag = 0;

	// It's easier to evaluate boolean expressions if we don't keep track of NOTs:
	// cut them out now and transform the required nodes to get an equivalent, NOT-less tree.
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
	{
		// If the boolean expression is a simple relational operation,
		// and there were an odd number of NOT's, then convert the
		// relational operator to its complement.
		cond_code->nodetype = getRelOpComplement(cond_code->nodetype);
	}
	else if (!isRelOp(cond_code->nodetype))
	{
		// If the boolean expression is more complex (one or more logical ops),
		// then we call this function.  This will recursively traverse the tree,
		// essentially performing De Morgan's Law transformations on any parts
		// of the tree that require it.

		// For example: !(x <= y && z > x) is transformed to (x > y || z <= x)
		// Relational and logical operators are changed to their respective complement.
		DMTransformTree(&cond_code, not_flag);
	}

	struct logic_node *temp_cond = NULL;
	if (!isRelOp(cond_code->nodetype))
	{
		// Configures the jump targets for all of the logical operator nodes in the tree.
		// Depending on the logical operator (OR/AND), short-circuit targets can differ.
		// All jump targets depend on the structure of the tree, so they require some degree
		// of work that is deserving on their own functions.
		configureLogicNodes(&c_list, &cond_code);
	}
	else
	{ 	
		// Special case:  Conditional code is a simple relational operation.
		// We'll setup a special logic node so we can still use the generic evalCond() function later.
		temp_cond = (struct logic_node*)create_logic_node(0, cond_code, NULL);
		appendToCondList(&c_list, temp_cond);
	}

	if(a->nodetype == IF)
	{
		// Evaluates the LHS and RHS of relational operators, generates incomplete un/conditional jump statements
		// that are handled later (setJumpStatement()) and are accessible from the global COND_LIST_HEAD pointer.
		evalCond(&c_list);

		// Generate true codeblock statements.
		unsigned int true_line_start = func->VMQ_data.quad_end_line + 1;
		struct VMQ_list_node *true_jump_line = NULL;
		eval(true_code);

		// Insert dummy string for end-of-true-code jump statement, save the line for later.
		appendToVMQList("");
		true_jump_line = func->VMQ_data.stmt_list_tail;

		// Generate false codeblock statements.
		unsigned int false_line_start = func->VMQ_data.quad_end_line + 1;
		eval(false_code);

		// Update the jump string for the end-of-true-codeblock jump
		sprintf(true_jump_line->VMQ_line, "j %d", func->VMQ_data.quad_end_line + 1);

		// Set all of the un/conditional jump statements
		setJumpStatements(&c_list, true_line_start, false_line_start);
	}
	else if(a->nodetype == WHILE)
	{
		unsigned int loop_back_line = func->VMQ_data.quad_end_line + 1;

		evalCond(&c_list);

		unsigned int true_line_start = func->VMQ_data.quad_end_line + 1;
		eval(true_code);
		sprintf(VMQ_line, "j %d", loop_back_line);
		appendToVMQList(VMQ_line);
		unsigned int false_line_start = func->VMQ_data.quad_end_line + 1;

		setJumpStatements(&c_list, true_line_start, false_line_start);
	}
	else
		yyerror("evalIf() - Encountered unknown nodetype");
}
