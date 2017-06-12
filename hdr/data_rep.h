/* Defines supporting data structures used for representing various types of data the compiler tracks */

#ifndef DATA_REP_H_
#define DATA_REP_H_

/*
 *	VMQ_loc for each struct below is set AFTER the src file has been parsed.
 *	As such, the construction functions don't take a parameter for the VMQ_loc member variable.
*/

struct intlit
{
	unsigned int VMQ_loc;
	int val;
};

struct fltlit
{
	unsigned int VMQ_loc; 
	double val;
};

struct strlit
{
	unsigned int VMQ_loc;
	char* val;
};

struct var
{
    unsigned int var_type;	// INT or FLOAT
	char* var_name;
    int isGlobal;	
	int isParam;
    unsigned int size;      // if (size > 1), var is an array
};

struct varref
{
	unsigned int VMQ_loc;
	struct var* val;
};

struct intlit* newIntLit(int n);

struct fltlit* newFltLit(double d);

struct strlit* newStrLit(char* str);

struct var* newVar(unsigned int var_type, char* name, int isGlobal, int isParam, unsigned int size);

struct varref* newVarRef(struct var** var_ptr);

#endif
