#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

enum{ SYMBOL_TABLE_SIZE = 9997 };

struct symbol_node
{
	char* symbol;
	struct symbol_node* next;
};

typedef struct symbol_node* SYMBOL_TABLE;

int hash(char* str);

struct symbol_node* lookup(SYMBOL_TABLE* symTab, char* symbol);

SYMBOL_TABLE createSymbolTable();

#endif
