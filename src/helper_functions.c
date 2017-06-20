#include <stdlib.h>
#include <stdio.h>
#include "helper_functions.h"
#include "parser.tab.h"

void init()
{
	INT_LIST_HEAD = NULL;
	FLT_LIST_HEAD = NULL;
	STR_LIST_HEAD = NULL;
	GLOBAL_VAR_LIST_HEAD = NULL;

	FUNC_LIST_HEAD = CURRENT_FUNC = NULL;
	
	pushScope(&SCOPE_STACK_HEAD);

	GLOBAL_SCOPE = SCOPE_STACK_HEAD;
}

void eval(struct AST_node* a)
{
    if(!a) return;
    
    if(DEBUG) { printf("Nodetype == %s\n", nodeTypeToString(a->nodetype)); fflush(stdout); }
    switch(a->nodetype)
    {
	case 0:		    	    
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
	
	case STREAMIN:	    
	case STREAMOUT:
	case ASSIGNOP:    
	case RETURN:											    
	default:	    eval(a->l);
			    eval(a->r);
    }
    
}

char* nodeTypeToString(int nodetype)
{
    char* str = NULL;
    switch(nodetype)
    {
	/* "Non-terminal" cases */
	case PROG:	    str = "PROG"; break;
	case FUNC_DEF:	    str = "FUNC_DEF"; break;
	case FUNC_DEFS:	    str = "FUNC_DEFS"; break;
	case ID_LIST:	    str = "ID_LIST"; break;
	case VAR_DEFS:	    str = "VAR_DEFS"; break;
	case FUNC_HEAD:	    str = "FUNC_HEAD"; break;
	case PARAM_LIST:    str = "PARAM_LIST"; break;
	case PARAMS:	    str = "PARAM"; break;
	case BLOCK:	    str = "BLOCK"; break;
	case STMTS:	    str = "STMTS"; break;
	case STMT:	    str = "STMT"; break;
	case INPUT:	    str = "INPUT"; break;
	case OUTPUT:	    str = "OUTPUT"; break;
	case EXPRS:	    str = "EXPRS"; break;
	case ADD_ASSIGN:    str = "ADD_ASSIGN"; break;
	case SUB_ASSIGN:    str = "SUB_ASSIGN"; break;
	case ADD:	    str = "ADD"; break;
	case SUB:	    str = "SUB"; break;
	case MUL:	    str = "MUL"; break;
	case DIV:	    str = "DIV"; break;
	case MOD:	    str = "MOD"; break;
	case FUNC_CALL:	    str = "FUNC_CALL"; break;
	case LT:	    str = "LT"; break;
	case GT:	    str = "GT"; break;
	case LTE:	    str = "LTE"; break;
	case GTE:	    str = "GTE"; break;
	case EQ:	    str = "EQ"; break;
	case NEQ:	    str = "NEQ"; break;
	/* "Terminal" tokens */
	case 0:		    str = "NULL(0)"; break;
	case ASSIGNOP:	    str = "ASSIGNOP"; break;
	case ADDOP:	    str = "ADDOP"; break;
	case MULOP:	    str = "MULOP"; break;
	case INCOP:	    str = "INCOP"; break;
	case INT_LITERAL:   str = "INT_LITERAL"; break;
	case FLT_LITERAL:   str = "FLT_LITERAL"; break;
	case STR_LITERAL:   str = "STR_LITERAL"; break;
	case STREAMIN:	    str = "STREAMIN"; break;
	case STREAMOUT:	    str = "STREAMOUT"; break;
	case ID:	    str = "ID"; break;
	case CIN:	    str = "CIN"; break;
	case COUT:	    str = "COUT"; break;
	case ELSE:	    str = "ELSE"; break;
	case ENDL:	    str = "ENDL"; break;
	case FLOAT:	    str = "FLOAT"; break;
	case IF:	    str = "IF"; break;
	case INT:	    str = "INT"; break;
	case RETURN:	    str = "RETURN"; break;
	case WHILE:	    str = "WHILE"; break;
	case VAR_DEC:	    str = "VAR_DEC"; break;
	case ARR_DEC:	    str = "ARR_DEC"; break;
	case VAR_ACCESS:    str = "VAR_ACCESS"; break;
	case ARR_ACCESS:    str = "ARR_ACCESS"; break;
	default:	    str = "!!UNKNOWN!!";
    }

    return str;
}
