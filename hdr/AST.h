#ifndef AST_H_
#define AST_H_

/* General AST_node struct */
struct AST_node
{
	int nodetype;
	struct AST_node* l;
	struct AST_node* r;
};

/* Used for INT_LITERAL terminal cases (i.e., AST leaves) */
struct int_node
{
	int nodetype;
	struct intlit* val;
};

/* Used for FLT_LITERAL terminal cases (i.e., AST leaves) */
struct flt_node
{
	int nodetype;
	struct fltlit* val;
};

/* Used for STR_LITERAL terminal cases (i.e., AST leaves) */
struct str_node
{
	int nodetype;
	struct strlit* val;
};

/* Used for ID terminal cases (i.e., AST leaves) */
struct var_node
{
	int nodetype;
	struct varref* val;
};

/* Special node for IF_ELSE/WHILE statements */
struct ctrl_node
{
	int nodetype;
	struct AST_node* c;	//	Pointer to condition code
	struct AST_node* t;	//	Pointer to true code
	struct AST_node* f;	//	Pointer to false code
};

struct AST_node* create_AST_node(int nodetype, struct AST_node* l, struct AST_node* r);
struct AST_node* create_int_node(int nodetype, struct intlit* val);
struct AST_node* create_flt_node(int nodetype, struct fltlit* val);
struct AST_node* create_str_node(int nodetype, struct strlit* val);
struct AST_node* create_var_node(int nodetype, struct varref* val);
struct AST_node* create_ctrl_node(int nodetype, struct AST_node* c, struct AST_node* t, struct AST_node* f);

#endif
