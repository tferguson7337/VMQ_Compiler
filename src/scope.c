#include <stdlib.h>
#include "error_handling.h"
#include "helper_functions.h"
#include "scope.h"

struct scope_stack_node* createNewScopeNode()
{
	struct scope_stack_node* pssn = malloc(sizeof(struct scope_stack_node));

	if(!pssn) { yyerror("\n\tcreateNewScopeNode() - Memory Allocation Failed!\n"); exit(-1); }

	pssn->symTab = createSymbolTable();
	pssn->next = NULL;
	pssn->isNewScope = 1;

	return pssn;
}

void pushScope()
{
    if(!SCOPE_STACK_HEAD) // Empty Stack
	GLOBAL_SCOPE = SCOPE_STACK_HEAD = createNewScopeNode();
    else
    {
	struct scope_stack_node* temp = SCOPE_STACK_HEAD;
	SCOPE_STACK_HEAD = createNewScopeNode();
	SCOPE_STACK_HEAD->next = temp;
    }
}

void popScope()
{
    if(!SCOPE_STACK_HEAD) // Empty Stack
	return;

    struct scope_stack_node* temp = SCOPE_STACK_HEAD;

    SCOPE_STACK_HEAD = SCOPE_STACK_HEAD->next;

    // Need to code in a function that will fully clean the symbol tables.
    // Any symbol table entry that has had a collision will leak memory with this current approach.
    free(temp->symTab);
    free(temp);
}
