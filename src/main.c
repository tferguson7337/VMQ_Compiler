#include <stdio.h>
#include <string.h>
#include "eval.h"
#include "fileIO.h"
#include "helper_functions.h"

int main(int argc, char** argv)
{
    extern FILE* yyin;
    extern int yyparse();

    init();

    setSourceFile(argc, &argv, &yyin);

    setDebugFlags(argc, &argv);

    yyparse();
    fclose(yyin);

    configureGlobalMemorySpace();
    configureLocalMemorySpaces();
    
    eval(AST_ROOT);

    if(DEBUG) dumpGlobalDataLists();

    FILE* qFile;
    setDestFile(argv[getFileIndex(argc, &argv)], &qFile);

    populateVMQFile(&qFile);
    fclose(qFile);

    return 0;
}
