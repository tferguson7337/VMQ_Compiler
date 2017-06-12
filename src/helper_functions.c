#include <stdlib.h>
#include "helper_functions.h"

void init()
{
	int_list_head = NULL;
	flt_list_head = NULL;
	str_list_head = NULL;
	global_var_list_head = NULL;

	func_list_head = current_func = NULL;
	
	pushScope(&scope_stack_head);

	global_scope = scope_stack_head;
}
