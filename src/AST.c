#include <stdlib.h>
#include "AST.h"
#include "error_handling.h"

struct AST_node* create_AST_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r)
{
	struct AST_node* pan = malloc(sizeof(struct AST_node));

	if(!pan) { yyerror("create_AST_node() - Memory Allocation Failed!"); exit(-1); }

	pan->nodetype = nodetype;
	pan->l = l;
	pan->r = r;

	return pan;
}

struct AST_node* create_int_node(unsigned int nodetype, struct intlit* val)
{
	struct int_node* pin = malloc(sizeof(struct int_node));

	if(!pin) { yyerror("create_int_node() - Memory Allocation Failed!"); exit(-1); }

	pin->nodetype = nodetype;
	pin->val = val;

	return (struct AST_node*)pin;
}

struct AST_node* create_flt_node(unsigned int nodetype, struct fltlit* val)
{
	struct flt_node* pfn = malloc(sizeof(struct flt_node));

	if(!pfn) { yyerror("create_flt_node() - Memory Allocation Failed!"); exit(-1); }

	pfn->nodetype = nodetype;
	pfn->val = val;

	return (struct AST_node*)pfn;
}

struct AST_node* create_str_node(unsigned int nodetype, struct strlit* val)
{
	struct str_node* psn = malloc(sizeof(struct str_node));

	if(!psn) { yyerror("create_str_node() - Memory Allocation Failed!"); exit(-1); }

	psn->nodetype = nodetype;
	psn->val = val;

	return (struct AST_node*)psn;
}

struct AST_node* create_var_node(unsigned int nodetype, struct var* val)
{
	struct var_node* pvn = malloc(sizeof(struct var_node));

	if(!pvn) { yyerror("create_var_node() - Memory Allocation Failed!"); exit(-1); }

	pvn->nodetype = nodetype;
	pvn->val = val;

	return (struct AST_node*)pvn;
}

struct AST_node* create_func_node(unsigned int nodetype, struct func_list_node* val)
{
    struct func_node* pfn = malloc(sizeof(struct func_node));

    if(!pfn) { yyerror("create_func_node() - Memory Allocation Failed!"); exit(-1); }

    pfn->nodetype = nodetype;
    pfn->val = val;

    return (struct AST_node*)pfn;
}

struct AST_node* create_ctrl_node(unsigned int nodetype, struct AST_node* c, struct AST_node* t, struct AST_node* f)
{
	struct ctrl_node* pcn = malloc(sizeof(struct ctrl_node));

	if(!pcn) { yyerror("create_ctrl_node() - Memory Allocation Failed!"); exit(-1); }

	pcn->nodetype = nodetype;
	pcn->c = c;
	pcn->t = t;
	pcn->f = f;

	return (struct AST_node*)pcn;
}

struct AST_node* create_logic_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r)
{
    struct logic_node* pln = malloc(sizeof(struct logic_node));

    if(!pln) { yyerror("create_logic_node() - Memory Allocation Failed!"); }

    pln->nodetype = nodetype;
    pln->l = l;
    pln->r = r;
    pln->line_start = 0;
    pln->sc_target = pln->t_target = pln->f_target = NULL;

    return (struct AST_node*)pln;
}

struct AST_node* create_relop_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r)
{
    struct relop_node* prn = malloc(sizeof(struct relop_node));

    if(!prn) { yyerror("create_relop_node() - Memory Allocation Failed!"); }

    prn->nodetype = nodetype;
    prn->l = l;
    prn->r = r;
    prn->cond_jump_stmt = prn->uncond_jump_stmt = NULL;

    return (struct AST_node*)prn;
}
