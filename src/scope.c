#include <stdlib.h>
#include "error_handling.h"
#include "scope.h"

struct scope_stack_node* createNewScopeNode()
{
	struct scope_stack_node* pssn = malloc(sizeof(struct scope_stack_node));

	if(!pssn) { yyerror("\n\tcreateNewScopeNode() - Memory Allocation Failed!\n"); exit(-1); }

	pssn->symTab = createSymbolTable();
	pssn->next = NULL;

	return pssn;
}

void pushScope(SCOPE* stack_head)
{
	if(!stack_head) { yyerror("\n\tpushScope() - NULL stack_head addr!\n"); exit(-1); }

	
	if(!(*stack_head))	//	Empty Stack
		*stack_head = createNewScopeNode();
	else
	{
		struct scope_stack_node* temp = *stack_head;
		*stack_head = createNewScopeNode();
		(*stack_head)->next = temp;
	}
}

void popScope(SCOPE* stack_head)
{
	if(!stack_head)	{ yyerror("\n\tpopScope() - NULL stack_head addr!\n"); exit(-1); }
	
	if(!(*stack_head))	//	Empty Stack
		return;

	struct scope_stack_node* temp = *stack_head;

	*stack_head = (*stack_head)->next;

	free(temp->symTab);
	free(temp);
}
