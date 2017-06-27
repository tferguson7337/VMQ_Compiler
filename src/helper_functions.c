#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "helper_functions.h"
#include "parser.tab.h"

void init()
{
    INT_LIST_HEAD = INT_LIST_TAIL = NULL;
    FLT_LIST_HEAD = FLT_LIST_TAIL = NULL;
    STR_LIST_HEAD = STR_LIST_TAIL = NULL;
    GLOBAL_VAR_LIST_HEAD = GLOBAL_VAR_LIST_TAIL = NULL;
    VMQ_MEM_LIST_HEAD = VMQ_MEM_LIST_TAIL = NULL;

    FUNC_LIST_HEAD = CURRENT_FUNC = NULL;
	
    pushScope(&SCOPE_STACK_HEAD);

    GLOBAL_SCOPE = SCOPE_STACK_HEAD;

    DEBUG = FLEX_DEBUG = BISON_DEBUG = 0;

    // Add 0 and 1 to global VMQ space to represent TRUE and FALSE.
    appendToIntList("0"); addToSymbolTable(&(GLOBAL_SCOPE->symTab), "0");
    appendToIntList("1"); addToSymbolTable(&(GLOBAL_SCOPE->symTab), "1");
}

void configureGlobalMemorySpace()
{
    /*
*   VMQ Global Memory Layout
*
*   000 0	    ; Literal integer of value 0, stored at addr 000, represents FALSE
*   002 1	    ; Literal integer of value 1, stored at addr 002, represents TRUE
*   004 <flt_val>   ; Literal float values will be stored after initial TRUE and FALSE
*   008 <flt_var>   ; Space for any float variables comes after literal floats
*   012 <int_val>   ; Literal integer values will be stored after float values
*   014 <int_var>   ; Space for any integer variables comes after literal integers
*   016 <str_val>   ; Literal string values will be stored after integer values
*
*   Per VMQ Specifications...
*
*   Literal float values take up 32-bits (4 bytes), and must be aligned on a memory addr that is evenly divisible by 4.
*   Literal integer values take up 16-bits (2 bytes), and must be aligned on a memory addr that is evenly divisible by 2.
*   Literal strings values are null-terminated, and have no restrictions on memory addr alignment.
*   This particular layout optimizes VMQ global memory allocation, so that no memory padding (i.e., wasting memory) is needed
    */

    if(DEBUG) { printf("CONFIGURING GLOBAL MEMORY SPACE..."); fflush(stdout); }

    // Literal ints 0 and 1 will always be in the int list - set their addrs first.
    INT_LIST_HEAD->pil->VMQ_loc = 0;	    appendToVMQMemList(INT_LITERAL, INT_LIST_HEAD);
    INT_LIST_HEAD->next->pil->VMQ_loc = 2;  appendToVMQMemList(INT_LITERAL, INT_LIST_HEAD->next);
    
    unsigned int mem_addr = 4;

    // Literal floats come next.
    struct flt_list_node* pfln = FLT_LIST_HEAD;
    while(pfln)
    {
	pfln->pfl->VMQ_loc = mem_addr;
	appendToVMQMemList(FLT_LITERAL, pfln);
	mem_addr += VMQ_FLT_SIZE;
	pfln = pfln->next;
    }

    // Scan global var list for float vars, setting their VMQ_loc values.
   /* 
    *	Note that variables are merely allocated space in global memory and will not
    *	appear as literals do in the initial data list at the beginning of each VMQ file
    */
    struct var_list_node* pvln = GLOBAL_VAR_LIST_HEAD;
    while(pvln)
    {
	if(pvln->pvr->val->var_type == FLOAT)
	{
	    pvln->pvr->VMQ_loc = mem_addr;
	    mem_addr += VMQ_FLT_SIZE * pvln->pvr->val->size;
	}
	pvln = pvln->next;
    }

    // Literal integers come next - recall that first two list elements were already handled.
    struct int_list_node* piln = INT_LIST_HEAD->next->next;
    while(piln)
    {
	piln->pil->VMQ_loc = mem_addr;
	appendToVMQMemList(INT_LITERAL, piln);
	mem_addr += VMQ_INT_SIZE;
	piln = piln->next;
    }

    // Rescan global var list for int vars; set their VMQ_loc values.
    pvln = GLOBAL_VAR_LIST_HEAD;
    while(pvln)
    {
	if(pvln->pvr->val->var_type == INT)
	{
	    pvln->pvr->VMQ_loc = mem_addr;
	    mem_addr += VMQ_INT_SIZE * pvln->pvr->val->size;
	}

	pvln = pvln->next;
    }

    // Literal strings come next
    struct str_list_node* psln = STR_LIST_HEAD;
    while(psln)
    {
	psln->psl->VMQ_loc = mem_addr;
	appendToVMQMemList(STR_LITERAL, psln);
	if(strcmp(psln->psl->val, "\\n") == 0)
	    mem_addr += 2;
	else
	    mem_addr += strlen(psln->psl->val) - 1;
	psln = psln->next;
    }

    if(DEBUG) { printf("DONE!\n"); fflush(stdout); } 
}

void configureLocalMemorySpaces()
{
    CURRENT_FUNC = FUNC_LIST_HEAD;

    while(CURRENT_FUNC)
    {
	if(!CURRENT_FUNC->var_list_head)
	{
	    CURRENT_FUNC = CURRENT_FUNC->next;
	    continue;
	}

	struct var_list_node* list_ptr = CURRENT_FUNC->var_list_head;
	struct var_list_node* prev_node = NULL;
	struct varref* vref = NULL;
	struct var* v = NULL;

	// Traverse list, find first INT var or array of odd size - set VMQ_loc to 2;
	while(list_ptr)
	{
	    vref = list_ptr->pvr;
	    v = vref->val;
	    if(v->var_type == INT && v->size % 2)
	    {
		vref->VMQ_loc = 2;
		prev_node = list_ptr;
		break;
	    }

	    list_ptr = list_ptr->next;
	}

	if(!prev_node)
	{
	    // Function contains no INT vars or INT arrays of odd size.
	    // Memory cannot be ideally packed - just traverse the list
	    // and set VMQ_locs simply.

	    // Do the first variable so we can use prev_node in the loop
	    list_ptr = CURRENT_FUNC->var_list_head;
	    list_ptr->pvr->VMQ_loc = 2;
	    prev_node = list_ptr;
	    list_ptr = list_ptr->next;

	    while(list_ptr)
	    {
		vref = prev_node->pvr;
		v = vref->val;
		if(v->var_type == INT)
		    list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_INT_SIZE * v->size);
		else
		    list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_FLT_SIZE * v->size);

		prev_node = list_ptr;
		list_ptr = list_ptr->next;
	    }
	    
	}
	else
	{
	    // Scan for floats, set their VMQ_locs first.
	    list_ptr = CURRENT_FUNC->var_list_head;
	    while(list_ptr)
	    {
		vref = prev_node->pvr;
		v = vref->val;
		
		if(list_ptr->pvr->val->var_type == FLOAT)
		{
		    if(v->var_type == INT)
			list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_INT_SIZE * v->size);
		    else
			list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_FLT_SIZE * v->size);

		    prev_node = list_ptr;
		}

		list_ptr = list_ptr->next;
	    }

	    // Now scan for ints and set their VMQ_locs
	    list_ptr = CURRENT_FUNC->var_list_head;
	    while(list_ptr)
	    {
		vref = prev_node->pvr;
		v = vref->val;

		if(list_ptr->pvr->val->var_type == INT)
		{
		    if(v->var_type == INT)
			list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_INT_SIZE * v->size);
		    else
			list_ptr->pvr->VMQ_loc = vref->VMQ_loc + (VMQ_FLT_SIZE * v->size);

		    prev_node = list_ptr;
		}

		list_ptr = list_ptr->next;
	    }   
	}

	CURRENT_FUNC = CURRENT_FUNC->next;
    }
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
	    printf("%03d\t%-15s\t(0x%llX)\n", pfln->pfl->VMQ_loc, pfln->pfl->val, (unsigned long long)pfln);
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
	    printf("%03d\t%-15s\t(0x%llX)\n", piln->pil->VMQ_loc, piln->pil->val, (unsigned long long)piln);
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
	    if(strcmp(psln->psl->val, "\\n") == 0)
		printf("%03d\t%-30s\t(0x%llX)\n", psln->psl->VMQ_loc, "\"\\n\"", (unsigned long long)psln);
	    else
		printf("%03d\t%-30s\t(0x%llX)\n", psln->psl->VMQ_loc, psln->psl->val, (unsigned long long)psln);
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
	    printf("%03d\t", pvln->pvr->VMQ_loc);
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
	    printf("\t--> Dumping %s\'s vars:\n", pFuncList->func_name);
	    pvln = pFuncList->var_list_head;
	    if(!pvln)
		printf("\tEMPTY\n");
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

    printf("==========================\n");
    printf("Dumping VMQ MEMORY LIST\n");
    printf("==========================\n\n");

    struct VMQ_mem_node* pvmn = VMQ_MEM_LIST_HEAD;

    if(!pvmn)
	printf("EMPTY\n");
    else
	while(pvmn)
	{
	    switch(pvmn->nodetype)
	    {
		struct intlit* pil;
		struct fltlit* pfl;
		struct strlit* psl;
		case INT_LITERAL:   pil = ((struct int_list_node*)pvmn->node)->pil;
				    printf("%03d\t%-25s(0x%llX)\n", pil->VMQ_loc, pil->val, (unsigned long long)pvmn->node);
				    break;

		case FLT_LITERAL:   pfl = ((struct flt_list_node*)pvmn->node)->pfl;
				    printf("%03d\t%-25s(0x%llX)\n", pfl->VMQ_loc, pfl->val, (unsigned long long)pvmn->node);
				    break;

		case STR_LITERAL:   psl = ((struct str_list_node*)pvmn->node)->psl;
				    if(strcmp(psl->val, "\\n") == 0)
					printf("%03d\t\"\\n\"\t\t(0x%llX)\n", psl->VMQ_loc, (unsigned long long)pvmn->node);
				    else
					printf("%03d\t%-25s(0x%llX)\n", psl->VMQ_loc, psl->val, (unsigned long long)pvmn->node);
				    break;
	    }

	    pvmn = pvmn->next;
	}

    printf("==========================\n\n");
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
