#include "conditional_helper_functions.h"
#include <string.h>
#include "eval.h"

void DMTransformTree(struct AST_node **root, unsigned int not_flag)
{
	struct AST_node *l = (*root)->l, *r = (*root)->r, *del_node = NULL;

	unsigned int lhs_not_flag = not_flag;
	while (l->nodetype == NOT)
	{
		del_node = l;
		l = (*root)->l = del_node->l;
		free(del_node);
		lhs_not_flag = !lhs_not_flag;
	}

	if (isRelOp(l->nodetype) && lhs_not_flag)
		l->nodetype = getRelOpComplement(l->nodetype);
	else if (!isRelOp(l->nodetype))
	{
		if (lhs_not_flag)
			l->nodetype = (l->nodetype == AND) ? OR : AND;

		DMTransformTree(&l, lhs_not_flag);
	}

	unsigned int rhs_not_flag = not_flag;

	while (r->nodetype == NOT)
	{
		del_node = r;
		r = (*root)->r = del_node->l;
		free(del_node);
		rhs_not_flag = !rhs_not_flag;
	}

	if (isRelOp(r->nodetype) && rhs_not_flag)
		r->nodetype = getRelOpComplement(r->nodetype);
	else if (!isRelOp(r->nodetype))
	{
		if (rhs_not_flag)
			r->nodetype = (r->nodetype == AND) ? OR : AND;

		DMTransformTree(&r, rhs_not_flag);
	}
}

void configureLogicNodes(struct cond_list* list, struct AST_node **root)
{
	if (isRelOp((*root)->l->nodetype) || isRelOp((*root)->r->nodetype))
	{
		struct logic_node *logic_root = (struct logic_node *)(*root);
		logic_root->sc_target = getShortCircuitTarget(*root);
		logic_root->lhs_target = getLHSTarget(*root);
		logic_root->t_target = getTrueTarget();
		logic_root->f_target = getFalseTarget();

		appendToCondList(list, logic_root);
	}

	if (!isRelOp((*root)->l->nodetype))
	{
		pushLogicNode((struct logic_node *)(*root));
		configureLogicNodes(list, &((*root)->l));
		popLogicNode();
	}

	if (!isRelOp((*root)->r->nodetype))
		configureLogicNodes(list, &((*root)->r));
}

void setJumpStatements(struct cond_list* list, unsigned int true_jump_line, unsigned int false_jump_line)
{
	struct cond_list_node* ptr = list->head;
	while (ptr)
	{
		char line_str[8];

		struct logic_node *logic_ptr = ptr->val;
		struct relop_node *lhs;
		if (logic_ptr->l && isRelOp(logic_ptr->l->nodetype))
			lhs = ((struct relop_node *)logic_ptr->l);
		else
			lhs = NULL;

		struct relop_node *rhs;
		if (logic_ptr->r && isRelOp(logic_ptr->r->nodetype))
			rhs = ((struct relop_node *)logic_ptr->r);
		else
			rhs = NULL;

		char *cjump_stmt = NULL, *ujump_stmt = NULL;
		unsigned int jump_target;

		// Set jump statement for LHS relop.
		if (lhs)
		{
			if (lhs->cond_jump_stmt && *(lhs->cond_jump_stmt))
				cjump_stmt = *(lhs->cond_jump_stmt);
			else
				yyerror("evalIf() - Relational operation has no conditional jump line!");

			if (lhs->uncond_jump_stmt && *(lhs->uncond_jump_stmt))
				ujump_stmt = *(lhs->uncond_jump_stmt);

			// Get conditional jump target
			if (logic_ptr->nodetype == AND)
			{
				if (isSupportedRelop(lhs->nodetype))
				{ // Continue evaluation of RHS, no short circuit for the conditional target
					jump_target = logic_ptr->lhs_target->line_start;
				}
				else // lhs->nodetype == LTE, GTE, or NEQ - short circuit
				{
					if (logic_ptr->sc_target)
						jump_target = logic_ptr->sc_target->line_start;
					else
						jump_target = false_jump_line;
				}
			}
			else if(logic_ptr->nodetype == OR)
			{
				// Short circuit
				if (isSupportedRelop(lhs->nodetype))
				{
					if (logic_ptr->sc_target)
						jump_target = logic_ptr->sc_target->line_start;
					else
						jump_target = true_jump_line;
				}
				else // lhs->nodetype = LTE, GTE, or NEQ
				{	// Continue evaluation of RHS, no short circuit for the conditional target
					jump_target = logic_ptr->lhs_target->line_start;
				}
			}
			else // logic_ptr->nodetype == 0
			{
				// Boolean expression is a single relop
				if(isSupportedRelop(lhs->nodetype))
					jump_target = true_jump_line;
				else
					jump_target = false_jump_line;
			}

			// Generate the rest of the conditional jump statement, replace the original.
			sprintf(line_str, " %d", jump_target);
			strcat(cjump_stmt, line_str);

			// If there is an unconditional op, get the jump target and generate the command.
			if (ujump_stmt)
			{
				/*
                    For the LHS, unconditional jumps only occur in the cases when the conditional
                    jump target is not a short circuit jump.

                    When logic_ptr->nodetype == AND && (lhs->nodetype == LT, GT, or EQ), the unconditional
                    jump is the short circuit for the AND.

                    When logic_ptr->nodetype == OR && (lhs->nodetype == LTE, GTE, or NEQ), the unconditional
                    jump is the short circuit for the OR.

                    If the conditional jump results in a short circuit when it evaluates to true, then there
                    will be no unconditional jump statement, and this block of code will not be run.
		        */

				if (logic_ptr->nodetype == AND && isSupportedRelop(lhs->nodetype))
					jump_target = (logic_ptr->sc_target) ? ((struct relop_node *)logic_ptr->sc_target->l)->line_start : false_jump_line;
				else if (logic_ptr->nodetype == AND && isUnsupportedRelop(lhs->nodetype))
					jump_target = (logic_ptr->t_target) ? ((struct relop_node*)logic_ptr->t_target->l)->line_start : true_jump_line;
				else if (logic_ptr->nodetype == OR && isSupportedRelop(lhs->nodetype))
					jump_target = (logic_ptr->sc_target) ? ((struct relop_node *)logic_ptr->sc_target->l)->line_start : true_jump_line;
				else if (logic_ptr->nodetype == OR && isUnsupportedRelop(lhs->nodetype))
					jump_target = (logic_ptr->f_target) ? ((struct relop_node *)logic_ptr->f_target->l)->line_start : false_jump_line;
				else // logic_ptr->nodetype == 0
					jump_target = (isSupportedRelop(lhs->nodetype)) ? false_jump_line : true_jump_line;

				sprintf(ujump_stmt, "j %d", jump_target);
			}
		}

		if (rhs && isRelOp(rhs->nodetype))
		{
			cjump_stmt = NULL, ujump_stmt = NULL;

			if (rhs->cond_jump_stmt && *(rhs->cond_jump_stmt))
				cjump_stmt = *(rhs->cond_jump_stmt);
			else
				yyerror("setJumpStatements() - Relational operation has no conditional jump line!");

			if (rhs->uncond_jump_stmt && *(rhs->uncond_jump_stmt))
				ujump_stmt = *(rhs->uncond_jump_stmt);

			// Get conditional jump target
			if (logic_ptr->nodetype == AND || logic_ptr->nodetype == OR)
			{
				if (isSupportedRelop(rhs->nodetype))
					jump_target = (logic_ptr->t_target) ? logic_ptr->t_target->line_start : true_jump_line;
				else // rhs->nodetype == LTE, GTE, or NEQ
					jump_target = (logic_ptr->f_target) ? logic_ptr->f_target->line_start : false_jump_line;
			}

			// Generate the rest of the conditional jump statement, replace the original.
			sprintf(line_str, " %d", jump_target);
			strcat(cjump_stmt, line_str);

			if (ujump_stmt)
			{
				/*
		    	For the rhs, unconditional jumps only occur in the cases when the conditional
		    	jump target is not a short circuit jump.

		    	When logic_ptr->nodetype == AND && (rhs->nodetype == LT, GT, or EQ), the unconditional
		    	jump is the short circuit for the AND.

		    	When logic_ptr->nodetype == OR && (rhs->nodetype == LTE, GTE, or NEQ), the unconditional
		    	jump is the short circuit for the OR.

		    	If the conditional jump results in a short circuit when it evaluates to true, then there
		    	will be no unconditional jump statement, and this block of code will not be run.
				*/
				
				if (logic_ptr->nodetype == AND && isSupportedRelop(rhs->nodetype))
					jump_target = (logic_ptr->sc_target) ? logic_ptr->sc_target->line_start : false_jump_line;
				else if (logic_ptr->nodetype == AND && isUnsupportedRelop(rhs->nodetype))
					jump_target = (logic_ptr->t_target) ? logic_ptr->t_target->line_start : true_jump_line;
				else if (logic_ptr->nodetype == OR && isUnsupportedRelop(rhs->nodetype))
					jump_target = (logic_ptr->sc_target) ? logic_ptr->sc_target->line_start : true_jump_line;
				else if (logic_ptr->nodetype == OR && isSupportedRelop(rhs->nodetype))
					jump_target = (logic_ptr->f_target) ? logic_ptr->f_target->line_start : false_jump_line;

				sprintf(ujump_stmt, "j %d", jump_target);
			}
		}
		
		// Move to the next node, free the one we finished working on.
		struct cond_list_node* del = ptr;
		ptr = ptr->next;
		free(logic_ptr);
		free(del);
	}
	list->head = list->tail = NULL;
}
