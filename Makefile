CDIR = src
HDIR = hdr

cVMQ:	${CDIR}/lexer.c ${CDIR}/parser.tab.c \
		${CDIR}/main.c ${CDIR}/AST.c ${CDIR}/data_lists.c ${CDIR}/data_rep.c \
		${CDIR}/error_handling.c ${CDIR}/eval.c ${CDIR}/fileIO.c \
		${CDIR}/helper_functions.c ${CDIR}/scope.c ${CDIR}/symbol_table.c \
		${HDIR}/AST.h ${HDIR}/data_lists.h  ${HDIR}/data_rep.h ${HDIR}/error_handling.h \
		${HDIR}/eval.h ${HDIR}/fileIO.h ${HDIR}/helper_functions.h ${HDIR}/parser.tab.h \
		${HDIR}/scope.h ${HDIR}/symbol_table.h
		cc -g -I ${CDIR} -I ${HDIR} -o $@ ${CDIR}/parser.tab.c ${CDIR}/lexer.c \
		${CDIR}/main.c ${CDIR}/AST.c ${CDIR}/error_handling.c ${CDIR}/fileIO.c \
		${CDIR}/data_lists.c ${CDIR}/data_rep.c ${CDIR}/helper_functions.c \
		${CDIR}/scope.c ${CDIR}/symbol_table.c -lm

${CDIR}/lexer.c:	${CDIR}/lexer.l
		flex -o ${CDIR}/lexer.c ${CDIR}/lexer.l

${HDIR}/parser.tab.h:	${CDIR}/parser.y
		bison -o ${HDIR}/parser.tab.c -d ${CDIR}/parser.y
		rm -f ${HDIR}/parser.tab.c

${CDIR}/parser.tab.c:	${CDIR}/parser.y
		bison -o ${CDIR}/parser.tab.c ${CDIR}/parser.y

clean:
		rm -f cVMQ ${CDIR}/lexer.c \
		${CDIR}/parser.tab.c \
		${HDIR}/parser.tab.h

		rm -r -f *.dSYM

clean_q:
		rm -f *.q
