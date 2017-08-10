#ifndef AST_H_
#define AST_H_

/* General AST_node struct */
struct AST_node
{
	unsigned int nodetype;
	struct AST_node* l;
	struct AST_node* r;
};

/* Used for INT_LITERAL terminal cases (i.e., AST leaves) */
struct int_node
{
	unsigned int nodetype;
	struct intlit* val;
};

/* Used for FLT_LITERAL terminal cases (i.e., AST leaves) */
struct flt_node
{
	unsigned int nodetype;
	struct fltlit* val;
};

/* Used for STR_LITERAL terminal cases (i.e., AST leaves) */
struct str_node
{
	unsigned int nodetype;
	struct strlit* val;
};

/* Used for ID terminal cases (i.e., AST leaves) */
struct var_node
{
	unsigned int nodetype;
	struct var* val;
};

struct func_node
{
    unsigned int nodetype;
    struct func_list_node* val;
};

/* Special node for IF_ELSE/WHILE statements */
struct ctrl_node
{
	unsigned int nodetype;
	struct AST_node* c;	//	Pointer to condition code
	struct AST_node* t;	//	Pointer to true code
	struct AST_node* f;	//	Pointer to false code
};

/* Special node for boolean expressions */
struct logic_node
{
    unsigned int nodetype;
    struct AST_node* l, *r;
    struct relop_node* sc_target, *t_target, *f_target, *lhs_target;
};

struct relop_node
{
    unsigned int nodetype;
    struct AST_node* l, *r;
	unsigned int line_start;
    struct VMQ_list_node** cond_jump_stmt, **uncond_jump_stmt;
};

struct AST_node* create_AST_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r);
struct AST_node* create_int_node(unsigned int nodetype, struct intlit* val);
struct AST_node* create_flt_node(unsigned int nodetype, struct fltlit* val);
struct AST_node* create_str_node(unsigned int nodetype, struct strlit* val);
struct AST_node* create_var_node(unsigned int nodetype, struct var* val);
struct AST_node* create_func_node(unsigned int nodetype, struct func_list_node* val);
struct AST_node* create_ctrl_node(unsigned int nodetype, struct AST_node* c, struct AST_node* t, struct AST_node* f);
struct AST_node* create_logic_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r);
struct AST_node* create_relop_node(unsigned int nodetype, struct AST_node* l, struct AST_node* r);

#endif
