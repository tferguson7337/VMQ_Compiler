#include "error_handling.h"

void yyerror(char* str)
{
	printf("Error - %s\n", str);

	exit(-1);
}
