#include "eval.h"

void evalIf(struct AST_node *a)
{    
    struct AST_node* cond = ((struct ctrl_node*)a)->c;
    struct AST_node* t_code = ((struct ctrl_node*)a)->t;
    struct AST_node* f_code = ((struct ctrl_node*)a)->f;

    
}
