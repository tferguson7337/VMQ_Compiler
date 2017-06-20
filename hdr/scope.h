#ifndef SCOPE_H_
#define SCOPE_H_

#include "symbol_table.h"

struct scope_stack_node
{
    SYMBOL_TABLE symTab;
    struct scope_stack_node* next;
    int isNewScope;
};

typedef struct scope_stack_node* SCOPE;

struct scope_stack_node* createNewScope();

void pushScope();

void popScope();

#endif
