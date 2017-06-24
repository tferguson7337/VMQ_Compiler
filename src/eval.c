#include <stdlib.h>
#include "eval.h"
#include "parser.tab.h"

void eval(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    switch(a->nodetype)
    {
	case INT_LITERAL:
	case FLT_LITERAL:
	case STR_LITERAL:
	case ID:
	case ENDL:
	case VAR_DEC:
	case ARR_DEC:
	case VAR_ACCESS:
	case ARR_ACCESS:    return;

	case IF:
	case WHILE:	    eval(((struct ctrl_node*)a)->c);
			    eval(((struct ctrl_node*)a)->t);
			    eval(((struct ctrl_node*)a)->f);
			    break;

	default:	    eval(a->l); eval(a->r);
    }
}
