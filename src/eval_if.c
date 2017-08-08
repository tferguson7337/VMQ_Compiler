#include "eval.h"

void evalIf(struct AST_node *a)
{    
    struct AST_node* cond_code = ((struct ctrl_node*)a)->c;
    struct AST_node* true_code = ((struct ctrl_node*)a)->t;
    struct AST_node* false_code = ((struct ctrl_node*)a)->f;

    struct func_list_node* func = CURRENT_FUNC;

    unsigned int not_count = 0;
    if(cond_code->nodetype == NOT)
    {
	struct AST_node* del_node = NULL;
	while(cond_code->nodetype == NOT)
	{
	    del_node = cond_code;
	    cond_code = cond_code->l;
	    free(del_node);
	    not_count ^= 1;
	}

	((struct ctrl_node*)a)->c = cond_code;
    }

    if(isRelOp(cond_code->nodetype) && not_count)
	cond_code->nodetype = (cond_code->nodetype == AND) ? OR : AND;
    else if(!isRelOp(cond_code->nodetype))
	DMTransformTree(&cond_code, not_count);
    
    struct func_list_node* temp = malloc(sizeof(struct func_list_node));

    temp->VMQ_data.stmt_count = 0;
    temp->VMQ_data.quad_start_line = temp->VMQ_data.quad_end_line = func->VMQ_data.quad_end_line + 1;
    
    if(!isRelOp(cond_code->nodetype))
	configureLogicNodes(&cond_code);

}

void DMTransformTree(struct AST_node** root, unsigned int not_count)
{
    // Special case - if root of boolean_expression tree is 1 or more NOTs
    if((*root)->nodetype == NOT)
    {
	struct AST_node* del_node = NULL;
	while((*root)->nodetype == NOT)
	{
	    del_node = *root;
	    root = &((*root)->l);
	    free(del_node);
	    not_count ^= 1;
	}

	if(isRelOp((*root)->nodetype) && not_count)
	   (*root)->nodetype = getRelOpComplement((*root)->nodetype);
	else if(!isRelOp((*root)->nodetype))
	    DMTransformTree(root, not_count);
	
	return;
    }

    struct AST_node* l = (*root)->l, *r = (*root)->r, *del_node = NULL;

    unsigned int lhs_not_count = not_count;
    while(l->nodetype == NOT)
    {
	del_node = l;
	l = (*root)->l = del_node->l;
	free(del_node);
	lhs_not_count ^= 1;
    }

    if(isRelOp(l->nodetype) && lhs_not_count)
	l->nodetype = getRelOpComplement(l->nodetype);
    else if(!isRelOp(l->nodetype))
    {
	if(lhs_not_count)
	    l->nodetype = (l->nodetype == AND) ? OR : AND;

	DMTransformTree(&l, lhs_not_count);
    }

    unsigned int rhs_not_count = not_count;

    while(r->nodetype == NOT)
    {
	del_node = r;
	r = (*root)->r = del_node->l;
	free(del_node);
	rhs_not_count ^= 1;
    }

    if(isRelOp(r->nodetype) && rhs_not_count)
	r->nodetype = getRelOpComplement(r->nodetype);
    else if(!isRelOp(r->nodetype))
    {
	if(rhs_not_count)
	    r->nodetype = (r->nodetype == AND) ? OR : AND;

	DMTransformTree(&r, rhs_not_count);
    }
}

void configureLogicNodes(struct AST_node** root)
{
    
}
