#include <stdlib.h>
#include <string.h>
#include "data_rep.h"
#include "error_handling.h"

struct intlit* newIntLit(char* val)
{
	struct intlit* pil = malloc(sizeof(struct intlit));

	if(!pil) { yyerror("\n\tnewIntLit() - Memory Allocation Failed!\n"); exit(-1); }

	pil->VMQ_loc = 0;
	pil->val = strdup(val);

	return pil;
}

struct fltlit* newFltLit(char* val)
{
	struct fltlit* pfl = malloc(sizeof(struct fltlit));
	
	if(!pfl) { yyerror("\n\tnewFltLit() - Memory Allocation Failed!\n"); exit(-1); }

	pfl->VMQ_loc = 0;
	pfl->val = strdup(val);

	return pfl;
}

struct strlit* newStrLit(char* str)
{
	struct strlit* psl = malloc(sizeof(struct strlit));

	if(!psl) { yyerror("\n\tnewStrLit() - Memory Allocation Failed!\n"); exit(-1); }

	psl->VMQ_loc = 0;
	psl->val = strdup(str);

	return psl;
}

struct var* newVar(unsigned int var_type, char* name, int isGlobal, int isParam, unsigned int size)
{
	struct var* pv = malloc(sizeof(struct var));

	if(!pv) { yyerror("\n\tnewVar() - Memory Allocation Failed!\n"); exit(-1); }

	pv->VMQ_loc = 0;
	pv->var_type = var_type;
	pv->var_name = strdup(name);
	pv->isGlobal = isGlobal;
	pv->isParam = isParam;
	pv->size = size;

	return pv;
}
