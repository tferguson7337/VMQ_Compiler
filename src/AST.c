#include <stdlib.h>
#include "AST.h"
#include "error_handling.h"

struct AST_node* create_AST_node(int nodetype, struct AST_node* l, struct AST_node* r)
{
	struct AST_node* pan = malloc(sizeof(struct AST_node));

	if(!pan) { yyerror("\n\tcreate_AST_node() - Memory Allocation Failed!\n"); exit(-1); }

	pan->nodetype = nodetype;
	pan->l = l;
	pan->r = r;

	return pan;
}

struct AST_node* create_int_node(int nodetype, struct intlit* val)
{
	struct int_node* pin = malloc(sizeof(struct int_node));

	if(!pin) { yyerror("\n\tcreate_int_node() - Memory Allocation Failed!\n"); exit(-1); }

	pin->nodetype = nodetype;
	pin->val = val;

	return (struct AST_node*)pin;
}

struct AST_node* create_flt_node(int nodetype, struct fltlit* val)
{
	struct flt_node* pfn = malloc(sizeof(struct flt_node));

	if(!pfn) { yyerror("\n\tcreate_flt_node() - Memory Allocation Failed!\n"); exit(-1); }

	pfn->nodetype = nodetype;
	pfn->val = val;

	return (struct AST_node*)pfn;
}

struct AST_node* create_str_node(int nodetype, struct strlit* val)
{
	struct str_node* psn = malloc(sizeof(struct str_node));

	if(!psn) { yyerror("\n\tcreate_str_node() - Memory Allocation Failed!\n"); exit(-1); }

	psn->nodetype = nodetype;
	psn->val = val;

	return (struct AST_node*)psn;
}

struct AST_node* create_var_node(int nodetype, struct varref* val)
{
	struct var_node* pvn = malloc(sizeof(struct var_node));

	if(!pvn) { yyerror("\n\tcreate_var_node() - Memory Allocation Failed!\n"); exit(-1); }

	pvn->nodetype = nodetype;
	pvn->val = val;

	return (struct AST_node*)pvn;
}

struct AST_node* create_func_node(int nodetype, struct func_list_node* val)
{
    struct func_node* pfn = malloc(sizeof(struct func_node));

    if(!pfn) { yyerror("\n\tcreate_func_node() - Memory Allocation Failed!\n"); exit(-1); }

    pfn->nodetype = nodetype;
    pfn->val = val;

    return (struct AST_node*)pfn;
}

struct AST_node* create_ctrl_node(int nodetype, struct AST_node* c, struct AST_node* t, struct AST_node* f)
{
	struct ctrl_node* pcn = malloc(sizeof(struct ctrl_node));

	if(!pcn) { yyerror("\n\tcreate_ctrl_node() - Memory Allocation Failed!\n"); exit(-1); }

	pcn->nodetype = nodetype;
	pcn->c = c;
	pcn->t = t;
	pcn->f = f;

	return (struct AST_node*)pcn;
}
