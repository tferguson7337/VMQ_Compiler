CDIR = src
HDIR = hdr
ERR_OUT = gcc_err

cVMQ:		${CDIR}/lexer.c ${CDIR}/parser.tab.c \
		${CDIR}/main.c ${CDIR}/AST.c ${CDIR}/data_lists.c ${CDIR}/data_rep.c \
		${CDIR}/error_handling.c ${CDIR}/eval.c ${CDIR}/eval_array.c \
		${CDIR}/eval_assign.c ${CDIR}/eval_function_call.c \
		${CDIR}/eval_if.c ${CDIR}/eval_incrementation.c ${CDIR}/eval_input.c \
		${CDIR}/eval_math.c ${CDIR}/eval_output.c ${CDIR}/eval_return.c \
		${CDIR}/eval_while.c ${CDIR}/fileIO.c ${CDIR}/helper_functions.c \
		${CDIR}/scope.c ${CDIR}/symbol_table.c \
		${HDIR}/AST.h ${HDIR}/data_lists.h  ${HDIR}/data_rep.h ${HDIR}/error_handling.h \
		${HDIR}/eval.h ${HDIR}/fileIO.h ${HDIR}/helper_functions.h ${HDIR}/parser.tab.h \
		${HDIR}/scope.h ${HDIR}/symbol_table.h
		gcc -Wall -Wextra -pedantic -Ofast -g -I ${CDIR} -I ${HDIR} -o $@ ${CDIR}/parser.tab.c ${CDIR}/lexer.c \
		${CDIR}/main.c ${CDIR}/AST.c ${CDIR}/error_handling.c ${CDIR}/eval.c \
		${CDIR}/eval_array.c ${CDIR}/eval_assign.c ${CDIR}/eval_function_call.c \
		${CDIR}/eval_if.c ${CDIR}/eval_incrementation.c ${CDIR}/eval_input.c \
		${CDIR}/eval_math.c ${CDIR}/eval_output.c ${CDIR}/eval_return.c \
		${CDIR}/eval_while.c ${CDIR}/fileIO.c ${CDIR}/data_lists.c \
		${CDIR}/data_rep.c ${CDIR}/helper_functions.c ${CDIR}/scope.c ${CDIR}/symbol_table.c -lm 2> ${ERR_OUT}

${CDIR}/lexer.c:	${CDIR}/lexer.l
		flex -o ${CDIR}/lexer.c ${CDIR}/lexer.l

${HDIR}/parser.tab.h:	${CDIR}/parser.y
		bison -o ${HDIR}/parser.tab.c -d ${CDIR}/parser.y
		rm -f ${HDIR}/parser.tab.c

${CDIR}/parser.tab.c:	${CDIR}/parser.y
		bison -o ${CDIR}/parser.tab.c ${CDIR}/parser.y

clean_all:	clean clean_q

clean:
		rm -f cVMQ ${CDIR}/lexer.c \
		${CDIR}/parser.tab.c \
		${HDIR}/parser.tab.h \
		${ERR_OUT}

		rm -r -f *.dSYM

clean_q:
		rm -f *.q
