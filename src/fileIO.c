#include "fileIO.h"
#include "error_handling.h"
#include "helper_functions.h"
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
/*
*   VMQ Global Memory Layout
*
*   000 0	    ; Integer of value 0, stored at addr 000, represents FALSE
*   002 1	    ; Integer of value 1, stored at addr 002, represents TRUE
*   004 <flt_val>   ; Literal float values will be stored after initial TRUE and FALSE
*   006 <int_val>   ; Literal integer values will be stored after float values
*   008 <str_val>   ; Literal string values will be stored after integer values
*
*   Per VMQ Specifications...
*
*   Literal float values take up 32-bits (4 bytes), and must be aligned on a memory addr that is evenly divisible by 4.
*   Literal integer values take up 16-bits (2 bytes), and must be aligned on a memory addr that is evenly divisible by 2.
*   Literal strings values are null-terminated, and have no restrictions on memory addr alignment.
*   This particular layout optimizes VMQ global memory allocation, so that no memory padding (i.e., wasting memory) is needed
*/  
    fprintf(*fp, "000 0\n");
    fprintf(*fp, "002 1\n");
    
    int VMQ_mem_addr = 4;
    struct flt_list_node* pfln = FLT_LIST_HEAD;

    while(pfln)
    {
	fprintf(*fp, "%03d %s\n", VMQ_mem_addr, pfln->pfl->val);
	VMQ_mem_addr += 4;
	pfln = pfln->next;
    }

    struct int_list_node* piln = INT_LIST_HEAD;

    while(piln)
    {
	fprintf(*fp, "%03d %s\n", VMQ_mem_addr, piln->pil->val);
	VMQ_mem_addr += 2;
	piln = piln->next;
    }

    struct str_list_node* psln = STR_LIST_HEAD;

    while(psln)
    {
	char* str = psln->psl->val;
	if(strcmp(str, "\\n") == 0)
	    fprintf(*fp, "%03d \"%s\"\n", VMQ_mem_addr, str);
	else
	    fprintf(*fp, "%03d %s\n", VMQ_mem_addr, str);
	if(strcmp(str, "\\n") == 0)
	    VMQ_mem_addr += 2;
	else
	    VMQ_mem_addr += strlen(str) - 1;
	psln = psln->next;
    }
    
}
