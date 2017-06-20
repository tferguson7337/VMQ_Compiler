#include <stdio.h>
#include "helper_functions.h"
#include <string.h>
#include "fileIO.h"

int main(int argc, char** argv)
{
    extern FILE* yyin;
    extern int yyparse();

    DEBUG = 0;
    for(int i = 1; i < argc; ++i)
    {
	if(strcmp(argv[i], "-d"))
	    DEBUG = 1;
    }

    setSourceFile(argc, &argv, &yyin);

//  Initialize various data structures.
    init();
    
    yyparse();

    fclose(yyin);

    eval(AST_ROOT);

    FILE* qFile;

    setDestFile(argv[getFileIndex(argc, &argv)], &qFile);

//  populateVMQFile(&qFile);

    fclose(qFile);

    return 0;
}
