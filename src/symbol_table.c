#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "helper_functions.h"
#include "symbol_table.h"

int hash(char* str)
{
	int n = 0;
	int len = strlen(str);

	for(int i = 0; i < len; ++i)
		n += str[i];

	return n % SYMBOL_TABLE_SIZE;
}

void addToSymbolTable(SYMBOL_TABLE* symTab, char* symbol)
{
    int idx = hash(symbol);   
    struct symbol_node* symPtr = &((*symTab)[idx]);
    if(!(symPtr->next))
	symPtr->symbol = strdup(symbol);
    else
    {
	while(symPtr->next)
	    symPtr = symPtr->next;

	symPtr = symPtr->next = malloc(sizeof(struct symbol_node));
	symPtr->symbol = strdup(symbol);
	symPtr->next = NULL;
    }
}

struct symbol_node* symbolTableLookup(char* symbol)
{
    struct scope_stack_node* scopePtr = SCOPE_STACK_HEAD;
    
    while(scopePtr)
    {	// Search in scopePtr's symbol table (Hash Table), starting with deepest scope.
	int idx = hash(symbol);
	struct symbol_node* symPtr = &((scopePtr->symTab)[idx]);
	while(symPtr && symPtr->symbol)
	{
	    if(strcmp(symPtr->symbol, symbol) == 0)
		return symPtr;
	    else
		symPtr = symPtr->next;
	}
	// If symbol was not found, check higher scopes.
	scopePtr = scopePtr->next;
    }

    // Symbol not found.
    return NULL;
}

SYMBOL_TABLE createSymbolTable()
{
	SYMBOL_TABLE symTab = malloc(SYMBOL_TABLE_SIZE * sizeof(struct symbol_node));

	for(int i = 0; i < SYMBOL_TABLE_SIZE; ++i)
	{
		(symTab[i]).symbol = NULL;
		(symTab[i]).next = NULL;
	}

	return symTab;
}
