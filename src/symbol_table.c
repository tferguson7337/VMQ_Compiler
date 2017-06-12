#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "symbol_table.h"

int hash(char* str)
{
	int n = 0;
	int len = strlen(str);

	for(int i = 0; i < len; ++i)
		n += str[i];

	return n % SYMBOL_TABLE_SIZE;
}

struct symbol_node* lookup(SYMBOL_TABLE* symTab, char* symbol)
{
	if(!symTab || !(*symTab)) { yyerror("\n\tlookup() - NULL Symbol Table Address\n"); exit(-1); }

	int idx = hash(symbol);

	struct symbol_node* psn = &((*symTab)[idx]);

	// Search for symbol
	while(psn)
	{
		// Symbol found, return the node.
		if(strcmp(psn->symbol, symbol) == 0)
			return psn;
		
		// Hit last node, no match - break.
		if(!(psn->next))
			break;

		psn = psn->next;
	}

	// Symbol not in symbol table, add it and return it.
	psn = psn->next = malloc(sizeof(struct symbol_node));

	psn->symbol = strdup(symbol);
	psn->next = NULL;
	
	return psn;
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
