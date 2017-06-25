#include <stdlib.h>
#include "eval.h"
#include "parser.tab.h"

void eval(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    switch(a->nodetype)
    {
	// Higher level statement cases.
	case RETURN:	    evalReturnStatement(a);	break;
	case IF:	    evalIfStatement(a);		break;
	case WHILE:	    evalWhileStatement(a);	break;
	case INPUT:	    evalInputStatement(a);	break;
	case OUTPUT:	    evalOutputStatement(a);	break;

	// ASSIGNOP
	case ASSIGNOP:	    evalAssignOpStatement(a);	break;
	
	// INCOPs
	case ADD_ASSIGN:
	case SUB_ASSIGN:    evalIncOpStatement(a);	break;
	
	// ADDOPs
	case ADD:
	case SUB:	    evalAddOpStatement(a);	break;

	// MULOPs
	case MUL:
	case DIV:
	case MOD:	    evalMulOpStatement(a);	break;

	// Terminal cases, not handled here; return
	case INT_LITERAL:
	case FLT_LITERAL:
	case STR_LITERAL:
	case ID:
	case ENDL:
	case VAR_DEC:
	case ARR_DEC:
	case VAR_ACCESS:
	case ARR_ACCESS:    return;

	// All other non-terminal cases are traversed.
	default:	    eval(a->l); eval(a->r);
    }
}

void evalReturnStatement(struct AST_node* a)
{

}

void evalIfStatement(struct AST_node* a)
{

}

void evalWhileStatement(struct AST_node* a)
{

}

void evalInputStatement(struct AST_node* a)
{

}

void evalOutputStatement(struct AST_node* a)
{
    if(!a || a->nodetype == 0) return;

    switch(a->nodetype)
    {
	
    }
}

void evalAssignOpStatement(struct AST_node* a)
{

}

void evalIncOpStatement(struct AST_node* a)
{

}

void evalAddOpStatement(struct AST_node* a)
{

}

void evalMulOpStatement(struct AST_node* a)
{

}
