#include "fileIO.h"
#include "error_handling.h"
#include "helper_functions.h"
#include "parser.tab.h"
#include <string.h>
#include <stdio.h>

/* Scans through command line arguments, returning index of first encountered .cpp file (-1 if none are found). */
int getFileIndex(int count, char*** strs)
{
    int len;
    int idx;
    for (idx = 1; idx < count; ++idx)
    {
	len = strlen((*strs)[idx]);

        if(len > 4)
        {
            if((*strs)[idx][len-4] == '.' && (*strs)[idx][len-3] == 'c' && (*strs)[idx][len-2] == 'p' && (*strs)[idx][len-1] == 'p')
                break;
	}
    }

    if(idx == count) idx = -1;

    return idx;
}

/* Retrieves source file name from command line arguments, opens file using passed file pointer. */
void setSourceFile(int count, char*** args, FILE** fp)
{
    if(!fp)
	yyerror("setSourceFile - File pointer is NULL\n");

    int idx = getFileIndex(count, args);

    if(idx == -1)
        yyerror("No .cpp file provided.");
    
    char* file_name = (*args)[idx];

    (*fp) = fopen(file_name, "r");

    if(!(*fp))
        yyerror("File open procedure failed\n");
}

/* Creates new .q file in same directory as compiler, file name (excluding extension) will match the source file */
void setDestFile(char* src_file_name, FILE** fp)
{
    if(!fp)
	yyerror("setSourceFile - File pointer is NULL\n");
	
    int len = strlen(src_file_name);

/*  If source file is in different directory than compiler, we need to discard directory stuff from src_file_name */
    int idx;
    for(idx = len - 1; idx > 0; --idx)
    {
	if(src_file_name[idx - 1] == '/')
	    break;
    }

    char* dest_file_name = &(src_file_name[idx]);

    len = strlen(dest_file_name);

    dest_file_name[len-3] = 'q';
    dest_file_name[len-2] = '\0';

    (*fp) = fopen(dest_file_name, "w");

    if(!(*fp))
	yyerror("File open procedure failed\n");
}

void populateVMQFile(FILE** fp)
{
    struct VMQ_mem_node* pvmn = VMQ_MEM_LIST_HEAD;

    struct intlit* pil = NULL;
    struct fltlit* pfl = NULL;
    struct strlit* psl = NULL;
    struct varref* pvr = NULL;
    while(pvmn)
    {
	pil = NULL; pfl = NULL; psl = NULL; pvr = NULL;
	switch(pvmn->nodetype)
	{
	    case INT_LITERAL:	pil = ((struct int_list_node*)pvmn->node)->pil;
				fprintf(*fp, "%03d\t%s\n", pil->VMQ_loc, pil->val);
				break;

	    case FLT_LITERAL:	pfl = ((struct flt_list_node*)pvmn->node)->pfl;
				fprintf(*fp, "%03d\t%s\n", pfl->VMQ_loc, pfl->val);
				break;

	    case STR_LITERAL:	psl = ((struct str_list_node*)pvmn->node)->psl;
				if(strcmp(psl->val, "\\n") == 0)
				    fprintf(*fp, "%03d\t\"\\n\"\n", psl->VMQ_loc);
				else
				    fprintf(*fp, "%03d\t%s\n", psl->VMQ_loc, psl->val);
				break;

	    case INT:
	    case FLOAT:		pvr = ((struct var_list_node*)pvmn->node)->pvr;
				break;
	}

	pvmn = pvmn->next;
    }

    unsigned int globalMemSize = 0;

    if(pil)	    // if(pil), pil points to last value in global memory (INT_LITERAL)
	globalMemSize = pil->VMQ_loc + VMQ_INT_SIZE;
    else if(pfl)    // if(pfl), pfl points to last value in global memory (FLT_LITERAL)
	globalMemSize = pfl->VMQ_loc + VMQ_FLT_SIZE;
    else if(psl)    // if(psl), psl points to last value in global memory (STR_LITERAL)
	if(strcmp(psl->val, "\\n") == 0)
	    globalMemSize = psl->VMQ_loc + 2;
	else
	    globalMemSize = psl->VMQ_loc + strlen(psl->val) - 1;
    else	    // if(pvr), pvr points to last value in global memory (INT or FLOAT variable)
	if(pvr->val->var_type == INT)
	    globalMemSize = pvr->VMQ_loc + (VMQ_INT_SIZE * pvr->val->size);
	else
	    globalMemSize = pvr->VMQ_loc + (VMQ_FLT_SIZE * pvr->val->size);

    // Find main
    unsigned int start_line = 1;
    CURRENT_FUNC = FUNC_LIST_HEAD;
    while(CURRENT_FUNC)
    {
	if(strcmp(CURRENT_FUNC->func_name, "main") == 0)
	{
	    start_line = CURRENT_FUNC->VMQ_data.quad_start_line;
	    CURRENT_FUNC = FUNC_LIST_HEAD;
	    break;
	}
	
	CURRENT_FUNC = CURRENT_FUNC->next;
    }

    fprintf(*fp, "$ %d %d\n", start_line, globalMemSize);

    while(CURRENT_FUNC)
    {
	struct VMQ_list_node* pvln = CURRENT_FUNC->VMQ_data.stmt_list_head;
	while(pvln)
	{
	    fprintf(*fp, "%s\n", pvln->VMQ_line);
	    pvln = pvln->next;
	}

	CURRENT_FUNC = CURRENT_FUNC->next;
    }
}
