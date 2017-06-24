#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

    DEBUG = FLEX_DEBUG = BISON_DEBUG = 0;

    // Add 0 and 1 to global VMQ space to represent TRUE and FALSE.
    appendToIntList("0"); addToSymbolTable(&(GLOBAL_SCOPE->symTab), "0");
    appendToIntList("1"); addToSymbolTable(&(GLOBAL_SCOPE->symTab), "1");
}

void setDebugFlags(int argc, char*** argv)
{
    for(int i = 1; i < argc; ++i)
    {
	if(strcmp((*argv)[i], "-d") == 0)
	{
	    DEBUG = 1;
	    printf("DEBUG MODE ENABLED\n");
	    fflush(stdout);
	}
	else if(strcmp((*argv)[i], "-ld") == 0)
	{
	    FLEX_DEBUG = 1;
	    printf("LEXER DEBUG MODE ENABLED\n");
	    fflush(stdout);
	}
	else if(strcmp((*argv)[i], "-bd") == 0)
	{
	    BISON_DEBUG = 1;
	    printf("PARSER DEBUG MODE ENABLED\n");
	    fflush(stdout);
	}
    }
}

void dumpGlobalDataLists()
{
    printf("==========================\n");
    printf("Dumping LITERAL FLOAT list\n");
    printf("==========================\n\n");

    struct flt_list_node* pfln = FLT_LIST_HEAD;

    if(!pfln)
	printf("EMPTY\n");
    else
	while(pfln)
	{
	    printf("%-15s\t(0x%llX)\n", pfln->pfl->val, (unsigned long long)pfln);
	    pfln = pfln->next;
	}
    
    printf("==========================\n");
    printf("Dumping LITERAL INT list\n");
    printf("==========================\n\n");

    struct int_list_node* piln = INT_LIST_HEAD;

    if(!piln)
	printf("EMPTY\n");
    else
	while(piln)
	{
	    printf("%-15s\t(0x%llX)\n", piln->pil->val, (unsigned long long)piln);
	    piln = piln->next;
	}

    printf("==========================\n");
    printf("Dumping LITERAL STR list\n");
    printf("==========================\n\n");
    
    struct str_list_node* psln = STR_LIST_HEAD;
    
    if(!psln)
	printf("EMPTY\n");
    else
	while(psln)
	{
	    printf("%-30s\t(0x%llX)\n", psln->psl->val, (unsigned long long)psln);
	    psln = psln->next;
	}

    printf("==========================\n");
    printf("Dumping GLOBAL VAR list\n");
    printf("==========================\n\n");

    struct var_list_node* pvln = GLOBAL_VAR_LIST_HEAD;

    if(!pvln)
	printf("EMPTY\n");
    else
	while(pvln)
	{
	    printf("%-15s\t", pvln->pvr->val->var_name);
	    printf("%-5s\t", nodeTypeToString(pvln->pvr->val->var_type));
	    printf("%-3d\t", pvln->pvr->val->size);
	    printf("(0x%llX)\n", (unsigned long long)pvln);
	    pvln = pvln->next;
	}
    
    printf("==========================\n");
    printf("Dumping FUNC LIST\n");
    printf("==========================\n\n");

    struct func_list_node* pFuncList = FUNC_LIST_HEAD;

    if(!pFuncList)
	printf("EMPTY\n");
    else
	while(pFuncList)
	{
	    printf("%-15s\t(0x%llX)\n", pFuncList->func_name, (unsigned long long)pFuncList);
	    printf("--> Dumping %s\'s vars:\n", pFuncList->func_name);
	    pvln = pFuncList->var_list;
	    if(!pvln)
		printf("EMPTY\n");
	    else
		while(pvln)
		{
		    printf("\t%-15s\t", pvln->pvr->val->var_name);
		    printf("%-5s\t", nodeTypeToString(pvln->pvr->val->var_type));
		    printf("%-3d\t", pvln->pvr->val->size);
		    printf("(0x%llX)\n", (unsigned long long)pvln);
		    pvln = pvln->next;
		}
	    pFuncList = pFuncList->next;
	}
    printf("==========================\n\n"); fflush(stdout);
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
	/* "Terminal"/keyword tokens */
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
