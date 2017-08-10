#include "eval.h"
#include "conditional_helper_functions.h"

void evalCond()
{
    if (DEBUG)
    {
        printf("\nevalCond() - COND_LIST_HEAD == ");
        if (COND_LIST_HEAD)
            printf("0x%llX\n", (unsigned long long)COND_LIST_HEAD);
        else
            printf("NULL\n");
        fflush(stdout);
    }
    struct cond_list_node *cond_ptr = COND_LIST_HEAD;
    unsigned int *cur_line_num = &(CURRENT_FUNC->VMQ_data.quad_end_line);

    if (DEBUG)
    {
        printf("Entering cond_ptr loop...\n");
        fflush(stdout);
    }

    while (cond_ptr)
    {
        struct logic_node *ptr = cond_ptr->val;

        printf("\tInitializing data structures...");
        fflush(stdout);

        unsigned int orig_temp_size = CURRENT_FUNC->VMQ_data.tempvar_cur_size;
        struct relop_node *l_relop, *r_relop;
        struct AST_node *lhs = NULL, *rhs = NULL;
        char relop_code;
        char *lhs_addr_mode = NULL, *rhs_addr_mode = NULL;
        unsigned int lhs_addr, lhs_type, rhs_addr, rhs_type;

        printf("Done!\n");
        fflush(stdout);

        if (ptr->l && isRelOp(ptr->l->nodetype))
            l_relop = ((struct relop_node *)ptr->l);
        else
            l_relop = NULL;

        if (ptr->r && isRelOp(ptr->r->nodetype))
            r_relop = ((struct relop_node *)ptr->r);
        else
            r_relop = NULL;

        if (!l_relop && !r_relop)
            yyerror("evalCond() - A logic_node with no relop_node children ended up in the conditional list!");

        // Special case for when the boolean expression consists of a single relational operation.
        if (ptr->nodetype == 0)
        {
            if (DEBUG)
            {
                printf("\tControl has entered special case (logic_node type == 0)\n");
                fflush(stdout);
            }

            lhs = l_relop->l;
            rhs = l_relop->r;
            l_relop->line_start = *cur_line_num + 1;

            if (DEBUG)
            {
                printf("\tEntering LHS switch...");
                fflush(stdout);
            }
            switch (lhs->nodetype)
            {
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case MOD:
                evalMath(lhs);
                lhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                lhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                lhs_addr_mode = "/-";
                break;

            case VAR_ACCESS:
                lhs_addr = ((struct var_node *)lhs)->val->VMQ_loc;
                lhs_type = ((struct var_node *)lhs)->val->var_type;

                if (((struct var_node*)lhs)->val->isGlobal)
                    lhs_addr_mode = "";
                else
                    lhs_addr_mode = (((struct var_node*)lhs)->val->isParam) ? "@/" : "/-";
                break;

            case FUNC_CALL:
                evalFuncCall(lhs, ((struct func_node *)lhs)->val->param_list_tail);
                lhs_type = ((struct func_node *)lhs)->val->return_type;
                lhs_addr = (lhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                lhs_addr_mode = "/-";
                sprintf(VMQ_line, "c /-%d %d", lhs_addr, ((struct func_node *)lhs)->val->VMQ_data.quad_start_line);
                appendToVMQList(VMQ_line);
                sprintf(VMQ_line, "^ %d", ((struct func_node *)lhs)->val->param_count * VMQ_ADDR_SIZE);
                appendToVMQList(VMQ_line);
                break;

            case INT_LITERAL:
            case FLT_LITERAL:
                lhs_type = (lhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                lhs_addr = (lhs_type == INT) ? (((struct int_node *)lhs)->val->VMQ_loc) : (((struct flt_node *)lhs)->val->VMQ_loc);
                lhs_addr_mode = "";
                break;

            case ARR_ACCESS:
                evalArrAccess(lhs);
                lhs_type = ((struct var_node *)lhs->l)->val->var_type;
                lhs_addr = getNewTempVar(ADDR);
                lhs_addr_mode = "@/-";
                break;
            }

            if (DEBUG)
            {
                printf("\tDone!\n");
                printf("\tEntering RHS switch...");
                fflush(stdout);
            }

            switch (rhs->nodetype)
            {
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case MOD:
                evalMath(rhs);
                rhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                rhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                rhs_addr_mode = "/-";
                break;

            case VAR_ACCESS:
                rhs_addr = ((struct var_node *)rhs)->val->VMQ_loc;
                rhs_type = ((struct var_node *)rhs)->val->var_type;
                if (((struct var_node*)rhs)->val->isGlobal)
                    rhs_addr_mode = "";
                else
                    rhs_addr_mode = (((struct var_node*)rhs)->val->isParam) ? "@/" : "/-";
                break;

            case FUNC_CALL:
                evalFuncCall(rhs, ((struct func_node *)rhs)->val->param_list_tail);
                rhs_type = ((struct func_node *)rhs)->val->return_type;
                rhs_addr = (rhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                rhs_addr_mode = "/-";
                sprintf(VMQ_line, "c /-%d %d", rhs_addr, ((struct func_node *)rhs)->val->VMQ_data.quad_start_line);
                appendToVMQList(VMQ_line);
                sprintf(VMQ_line, "^ %d", ((struct func_node *)rhs)->val->param_count * VMQ_ADDR_SIZE);
                appendToVMQList(VMQ_line);
                break;

            case INT_LITERAL:
            case FLT_LITERAL:
                rhs_type = (rhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                rhs_addr = (rhs_type == INT) ? (((struct int_node *)rhs)->val->VMQ_loc) : (((struct flt_node *)rhs)->val->VMQ_loc);
                rhs_addr_mode = "";
                break;

            case ARR_ACCESS:
                evalArrAccess(rhs);
                rhs_type = ((struct var_node *)rhs->l)->val->var_type;
                rhs_addr = getNewTempVar(ADDR);
                rhs_addr_mode = "@/-";
                break;
            }

            if (DEBUG)
            {
                printf("\tDone!\n");
                fflush(stdout);
            }

            if (lhs_type != rhs_type)
            {

                // Types don't match, so we need to perform a cast before comparing values.
                unsigned int temp_addr = getNewTempVar(FLOAT);
                if (lhs_type == INT)
                {
                    sprintf(VMQ_line, "F %s%d /-%d", lhs_addr_mode, lhs_addr, temp_addr);
                    appendToVMQList(VMQ_line);
                    lhs_addr_mode = "/-";
                    lhs_addr = temp_addr;
                }
                else // rhs_type == INT
                {
                    sprintf(VMQ_line, "F %s%d /-%d", rhs_addr_mode, rhs_addr, temp_addr);
                    appendToVMQList(VMQ_line);
                    rhs_addr_mode = "/-";
                    rhs_addr = temp_addr;
                }

                if (l_relop->nodetype == LT || l_relop->nodetype == GTE)
                    relop_code = 'L';
                else if (l_relop->nodetype == GT || l_relop->nodetype == LTE)
                    relop_code = 'G';
                else
                    relop_code = 'E';

                sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                appendToVMQList(VMQ_line);
            }
            else // lhs_type == rhs_type
            {
                if (l_relop->nodetype == LT || l_relop->nodetype == GTE)
                    relop_code = 'L';
                else if (l_relop->nodetype == GT || l_relop->nodetype == LTE)
                    relop_code = 'G';
                else
                    relop_code = 'E';

                // Use the lowercase op code for INTs
                if (lhs_type == INT)
                    relop_code += 32;

                sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                appendToVMQList(VMQ_line);
            }

            l_relop->cond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;

            // False code comes first, is if the relational operator was a (<=), (>=), or (!=),
            // then we need to add an additional line.
            if (isUnsupportedRelop(l_relop->nodetype))
            {
                // Dummy unconditional jump statement
                appendToVMQList("");
                l_relop->uncond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;
            }
        }
        else // ptr->nodetype == AND || ptr->nodetype == OR
        {
            if (l_relop)
            {
                lhs = l_relop->l;
                rhs = l_relop->r;
                l_relop->line_start = *cur_line_num + 1;

                switch (lhs->nodetype)
                {
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                    evalMath(lhs);
                    lhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                    lhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                    lhs_addr_mode = "/-";
                    break;

                case VAR_ACCESS:
                    lhs_addr = ((struct var_node *)lhs)->val->VMQ_loc;
                    lhs_type = ((struct var_node *)lhs)->val->var_type;
                    if (((struct var_node*)lhs)->val->isGlobal)
                        lhs_addr_mode = "";
                    else
                        lhs_addr_mode = (((struct var_node*)lhs)->val->isParam) ? "@/" : "/-";
                    break;

                case FUNC_CALL:
                    evalFuncCall(lhs, ((struct func_node *)lhs)->val->param_list_tail);
                    lhs_type = ((struct func_node *)lhs)->val->return_type;
                    lhs_addr = (lhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                    lhs_addr_mode = "/-";
                    sprintf(VMQ_line, "c /-%d %d", lhs_addr, ((struct func_node *)lhs)->val->VMQ_data.quad_start_line);
                    appendToVMQList(VMQ_line);
                    sprintf(VMQ_line, "^ %d", ((struct func_node *)lhs)->val->param_count * VMQ_ADDR_SIZE);
                    appendToVMQList(VMQ_line);
                    break;

                case INT_LITERAL:
                case FLT_LITERAL:
                    lhs_type = (lhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                    lhs_addr = (lhs_type == INT) ? (((struct int_node *)lhs)->val->VMQ_loc) : (((struct flt_node *)lhs)->val->VMQ_loc);
                    lhs_addr_mode = "";
                    break;

                case ARR_ACCESS:
                    evalArrAccess(lhs);
                    lhs_type = ((struct var_node *)lhs->l)->val->var_type;
                    lhs_addr = getNewTempVar(ADDR);
                    lhs_addr_mode = "@/-";
                    break;
                }

                switch (rhs->nodetype)
                {
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                    evalMath(rhs);
                    rhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                    rhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                    rhs_addr_mode = "/-";
                    break;

                case VAR_ACCESS:
                    rhs_addr = ((struct var_node *)rhs)->val->VMQ_loc;
                    rhs_type = ((struct var_node *)rhs)->val->var_type;
                    if (((struct var_node*)rhs)->val->isGlobal)
                        rhs_addr_mode = "";
                    else
                        rhs_addr_mode = (((struct var_node*)rhs)->val->isParam) ? "@/" : "/-";
                    break;

                case FUNC_CALL:
                    evalFuncCall(rhs, ((struct func_node *)rhs)->val->param_list_tail);
                    rhs_type = ((struct func_node *)rhs)->val->return_type;
                    rhs_addr = (rhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                    rhs_addr_mode = "/-";
                    sprintf(VMQ_line, "c /-%d %d", rhs_addr, ((struct func_node *)rhs)->val->VMQ_data.quad_start_line);
                    appendToVMQList(VMQ_line);
                    sprintf(VMQ_line, "^ %d", ((struct func_node *)rhs)->val->param_count * VMQ_ADDR_SIZE);
                    appendToVMQList(VMQ_line);
                    break;

                case INT_LITERAL:
                case FLT_LITERAL:
                    rhs_type = (rhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                    rhs_addr = (rhs_type == INT) ? (((struct int_node *)rhs)->val->VMQ_loc) : (((struct flt_node *)rhs)->val->VMQ_loc);
                    rhs_addr_mode = "";
                    break;

                case ARR_ACCESS:
                    evalArrAccess(rhs);
                    rhs_type = ((struct var_node *)rhs->l)->val->var_type;
                    rhs_addr = getNewTempVar(ADDR);
                    rhs_addr_mode = "@/-";
                    break;
                }

                if (lhs_type != rhs_type)
                {
                    // Types don't match, so we need to perform a cast before comparing values.
                    unsigned int temp_addr = getNewTempVar(FLOAT);
                    if (lhs_type == INT)
                    {
                        sprintf(VMQ_line, "F %s%d /-%d", lhs_addr_mode, lhs_addr, temp_addr);
                        appendToVMQList(VMQ_line);
                        lhs_addr_mode = "/-";
                        lhs_addr = temp_addr;
                    }
                    else // rhs_type == INT
                    {
                        sprintf(VMQ_line, "F %s%d /-%d", rhs_addr_mode, rhs_addr, temp_addr);
                        appendToVMQList(VMQ_line);
                        rhs_addr_mode = "/-";
                        rhs_addr = temp_addr;
                    }

                    if (l_relop->nodetype == LT || l_relop->nodetype == GTE)
                        relop_code = 'L';
                    else if (l_relop->nodetype == GT || l_relop->nodetype == LTE)
                        relop_code = 'G';
                    else
                        relop_code = 'E';

                    sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                    appendToVMQList(VMQ_line);
                }
                else // lhs_type == rhs_type
                {
                    if (l_relop->nodetype == LT || l_relop->nodetype == GTE)
                        relop_code = 'L';
                    else if (l_relop->nodetype == GT || l_relop->nodetype == LTE)
                        relop_code = 'G';
                    else
                        relop_code = 'E';

                    // Use the lowercase op code for INTs
                    if (lhs_type == INT)
                        relop_code += 32;

                    sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                    appendToVMQList(VMQ_line);
                }

                l_relop->cond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;

                // False code comes first, is if the relational operator was a (<=), (>=), or (!=),
                // then we need to add an additional line.
                if ((ptr->nodetype == AND && isSupportedRelop(l_relop->nodetype)) || (ptr->nodetype == OR && isUnsupportedRelop(l_relop->nodetype)))
                {
                    // Dummy unconditional jump statement
                    appendToVMQList("");
                    l_relop->uncond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;
                }
            }

            if (r_relop)
            {
                lhs = r_relop->l;
                rhs = r_relop->r;
                r_relop->line_start = *cur_line_num + 1;

                switch (lhs->nodetype)
                {
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                    evalMath(lhs);
                    lhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                    lhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                    lhs_addr_mode = "/-";
                    break;

                case VAR_ACCESS:
                    lhs_addr = ((struct var_node *)lhs)->val->VMQ_loc;
                    lhs_type = ((struct var_node *)lhs)->val->var_type;
                    if (((struct var_node*)lhs)->val->isGlobal)
                        lhs_addr_mode = "";
                    else
                        lhs_addr_mode = (((struct var_node*)lhs)->val->isParam) ? "@/" : "/-";
                    break;

                case FUNC_CALL:
                    evalFuncCall(lhs, ((struct func_node *)lhs)->val->param_list_tail);
                    lhs_type = ((struct func_node *)lhs)->val->return_type;
                    lhs_addr = (lhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                    lhs_addr_mode = "/-";
                    sprintf(VMQ_line, "c /-%d %d", lhs_addr, ((struct func_node *)lhs)->val->VMQ_data.quad_start_line);
                    appendToVMQList(VMQ_line);
                    sprintf(VMQ_line, "^ %d", ((struct func_node *)lhs)->val->param_count * VMQ_ADDR_SIZE);
                    appendToVMQList(VMQ_line);
                    break;

                case INT_LITERAL:
                case FLT_LITERAL:
                    lhs_type = (lhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                    lhs_addr = (lhs_type == INT) ? (((struct int_node *)lhs)->val->VMQ_loc) : (((struct flt_node *)lhs)->val->VMQ_loc);
                    lhs_addr_mode = "";
                    break;

                case ARR_ACCESS:
                    evalArrAccess(lhs);
                    lhs_type = ((struct var_node *)lhs->l)->val->var_type;
                    lhs_addr = getNewTempVar(ADDR);
                    lhs_addr_mode = "@/-";
                    break;
                }

                switch (rhs->nodetype)
                {
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case MOD:
                    evalMath(rhs);
                    rhs_addr = CURRENT_FUNC->VMQ_data.math_result.VMQ_loc;
                    rhs_type = CURRENT_FUNC->VMQ_data.math_result.type;
                    rhs_addr_mode = "/-";
                    break;

                case VAR_ACCESS:
                    rhs_addr = ((struct var_node *)rhs)->val->VMQ_loc;
                    rhs_type = ((struct var_node *)rhs)->val->var_type;
                    if (((struct var_node*)rhs)->val->isGlobal)
                        rhs_addr_mode = "";
                    else
                        rhs_addr_mode = (((struct var_node*)rhs)->val->isParam) ? "@/" : "/-";
                    break;

                case FUNC_CALL:
                    evalFuncCall(rhs, ((struct func_node *)rhs)->val->param_list_tail);
                    rhs_type = ((struct func_node *)rhs)->val->return_type;
                    rhs_addr = (rhs_type == INT) ? getNewTempVar(INT) : getNewTempVar(FLOAT);
                    rhs_addr_mode = "/-";
                    sprintf(VMQ_line, "c /-%d %d", rhs_addr, ((struct func_node *)rhs)->val->VMQ_data.quad_start_line);
                    appendToVMQList(VMQ_line);
                    sprintf(VMQ_line, "^ %d", ((struct func_node *)rhs)->val->param_count * VMQ_ADDR_SIZE);
                    appendToVMQList(VMQ_line);
                    break;

                case INT_LITERAL:
                case FLT_LITERAL:
                    rhs_type = (rhs->nodetype == INT_LITERAL) ? INT : FLOAT;
                    rhs_addr = (rhs_type == INT) ? (((struct int_node *)rhs)->val->VMQ_loc) : (((struct flt_node *)rhs)->val->VMQ_loc);
                    rhs_addr_mode = "";
                    break;

                case ARR_ACCESS:
                    evalArrAccess(rhs);
                    rhs_type = ((struct var_node *)rhs->l)->val->var_type;
                    rhs_addr = getNewTempVar(ADDR);
                    rhs_addr_mode = "@/-";
                    break;
                }

                if (lhs_type != rhs_type)
                {
                    // Types don't match, so we need to perform a cast before comparing values.
                    unsigned int temp_addr = getNewTempVar(FLOAT);
                    if (lhs_type == INT)
                    {
                        sprintf(VMQ_line, "F %s%d /-%d", lhs_addr_mode, lhs_addr, temp_addr);
                        appendToVMQList(VMQ_line);
                        lhs_addr_mode = "/-";
                        lhs_addr = temp_addr;
                    }
                    else // rhs_type == INT
                    {
                        sprintf(VMQ_line, "F %s%d /-%d", rhs_addr_mode, rhs_addr, temp_addr);
                        appendToVMQList(VMQ_line);
                        rhs_addr_mode = "/-";
                        rhs_addr = temp_addr;
                    }

                    if (r_relop->nodetype == LT || r_relop->nodetype == GTE)
                        relop_code = 'L';
                    else if (r_relop->nodetype == GT || r_relop->nodetype == LTE)
                        relop_code = 'G';
                    else
                        relop_code = 'E';

                    sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                    appendToVMQList(VMQ_line);
                }
                else // lhs_type == rhs_type
                {
                    if (r_relop->nodetype == LT || r_relop->nodetype == GTE)
                        relop_code = 'L';
                    else if (r_relop->nodetype == GT || r_relop->nodetype == LTE)
                        relop_code = 'G';
                    else
                        relop_code = 'E';

                    // Use the lowercase op code for INTs
                    if (lhs_type == INT)
                        relop_code += 32;

                    sprintf(VMQ_line, "%c %s%d %s%d", relop_code, lhs_addr_mode, lhs_addr, rhs_addr_mode, rhs_addr);
                    appendToVMQList(VMQ_line);
                }

                r_relop->cond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;

                appendToVMQList("");
                r_relop->uncond_jump_stmt = &CURRENT_FUNC->VMQ_data.stmt_list_tail->VMQ_line;
            }
        }

        while (orig_temp_size != CURRENT_FUNC->VMQ_data.tempvar_cur_size)
            freeTempVar();

        cond_ptr = cond_ptr->next;
    }
}
