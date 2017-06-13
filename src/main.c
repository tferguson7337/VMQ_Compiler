 #include <stdio.h>
#include "helper_functions.h"
// #include "parser.tab.h"
#include "fileIO.h"

int main(int argc, char** argv)
{
	extern FILE* yyin;
	extern int yyparse();

	setSourceFile(argc, &argv, &yyin);

//	Initialize various data structures.
	init();

	yyparse();

	fclose(yyin);

//	eval(AST_root);

	FILE* qFile;

	setDestFile(argv[getFileIndex(argc, &argv)], &qFile);

//	populateVMQFile(&qFile);

	fclose(qFile);

	return 0;
}
