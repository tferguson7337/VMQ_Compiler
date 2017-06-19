#include <stdlib.h>
#include "helper_functions.h"

void init()
{
	INT_LIST_HEAD = NULL;
	FLT_LIST_HEAD = NULL;
	STR_LIST_HEAD = NULL;
	GLOBAL_VAR_LIST_HEAD = NULL;

	FUNC_LIST_HEAD = CURRENT_FUNC = NULL;
	
	pushScope(&SCOPE_STACK_HEAD);

	GLOBAL_SCOPE = SCOPE_STACK_HEAD;
}
