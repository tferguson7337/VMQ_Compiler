#include "fileIO.h"
#include "error_handling.h"
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

/*	If source file is in different directory than compiler, we need to discard directory stuff from src_file_name */
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
