#ifndef COND_HELPER_FUNCS_
#define COND_HELPER_FUNCS_

// Conditional Helper Functions - Used in evalIf() and evalCond()
#include "AST.h"
#include "error_handling.h"
#include "eval.h"
#include "helper_functions.h"
#include "parser.tab.h"

void DMTransformTree(struct AST_node **root, unsigned int not_count);
void configureLogicNodes(struct cond_list* list, struct AST_node **root);
void setJumpStatements(struct cond_list* list, unsigned int true_jump_line, unsigned int false_jump_line);

static inline unsigned int isSupportedRelop(unsigned int type)
{
    return (type == LT || type == GT || type == EQ);
}

static inline unsigned int isUnsupportedRelop(unsigned int type)
{
    return !isSupportedRelop(type);
}

static inline unsigned int isRelOp(unsigned int type)
{
    return (type == LT || type == GT || type == EQ || type == LTE || type == GTE || type == NEQ);
}

static inline unsigned int getRelOpComplement(unsigned int type)
{
    unsigned int complement = 0;
    switch (type)
    {
    case LT:
        complement = GTE;
        break;

    case GT:
        complement = LTE;
        break;

    case LTE:
        complement = GT;
        break;

    case GTE:
        complement = LT;
        break;

    case EQ:
        complement = NEQ;
        break;

    case NEQ:
        complement = EQ;
        break;

    default:
        yyerror("getRelOpComplement() - invalid argument");
    }

    return complement;
}

static inline struct relop_node *getTrueTarget()
{
    struct logic_stack_node *ptr = LOGIC_STACK_HEAD;

    while (ptr && (ptr->val->nodetype != AND))
        ptr = ptr->next;

    if (!ptr)
        return NULL;
    else if (isRelOp(ptr->val->r->nodetype))
        return (struct relop_node*)ptr->val->r;
    else // ptr && !isRelOp(ptr->val->r->nodetype)
    {
        struct AST_node *target_ptr = ptr->val->r;
        while (!isRelOp(target_ptr->nodetype))
            target_ptr = target_ptr->l;

        return (struct relop_node*)target_ptr;
    }
}

static inline struct relop_node *getFalseTarget()
{
    struct logic_stack_node *ptr = LOGIC_STACK_HEAD;

    while (ptr && (ptr->val->nodetype != OR))
        ptr = ptr->next;

    if (!ptr)
        return NULL;
    else if (isRelOp(ptr->val->r->nodetype))
        return (struct relop_node*)ptr->val->r;
    else
    {
        struct AST_node *target_ptr = ptr->val->r;
        while (!isRelOp(target_ptr->nodetype))
            target_ptr = target_ptr->l;

        return (struct relop_node*)target_ptr;
    }
}

static inline struct relop_node *getLHSTarget(struct AST_node* a)
{
    struct AST_node* target_ptr = a->r;

    while(!isRelOp(target_ptr->nodetype))
        target_ptr = target_ptr->l;

    return (struct relop_node*)target_ptr;
}

static inline struct relop_node *getShortCircuitTarget(struct AST_node* a)
{
    if (a->nodetype == AND)
        return getFalseTarget();
    else // a->nodetype == OR
        return getTrueTarget();
}

#endif
