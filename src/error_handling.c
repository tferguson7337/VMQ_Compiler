#include "error_handling.h"

void yyerror(char* str)
{
	printf("\n\nError - %s\n", str);

	exit(-1);
}
