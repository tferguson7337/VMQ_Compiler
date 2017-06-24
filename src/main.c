#include <stdio.h>
#include <string.h>
#include "eval.h"
#include "fileIO.h"
#include "helper_functions.h"

int main(int argc, char** argv)
{
    extern FILE* yyin;
    extern int yyparse();
    
//  Initialize various data structures.
    init();

    setSourceFile(argc, &argv, &yyin);

    setDebugFlags(argc, &argv);

    yyparse();
    fclose(yyin);

    if(DEBUG) dumpGlobalDataLists();

    eval(AST_ROOT);

    FILE* qFile;

    setDestFile(argv[getFileIndex(argc, &argv)], &qFile);

    populateVMQFile(&qFile);

    fclose(qFile);

    return 0;
}
