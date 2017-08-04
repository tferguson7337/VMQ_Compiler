/* Defines supporting data structures used for representing various types of data the compiler tracks */

#ifndef DATA_REP_H_
#define DATA_REP_H_

/*
 *  VMQ_loc for each struct below is set AFTER the src file has been parsed.
 *  As such, the construction functions don't take a parameter for the VMQ_loc member variable.
*/

struct intlit
{
    unsigned int VMQ_loc;
    char* val;
};

struct fltlit
{
    unsigned int VMQ_loc; 
    char* val;
};

struct strlit
{
    unsigned int VMQ_loc;
    char* val;
};

struct var
{
    unsigned int VMQ_loc;   // Location of variable in VMQ memory space.
    unsigned int var_type;  // INT or FLOAT
    char* var_name;
    int isGlobal;	
    int isParam;
    unsigned int size;      // if (size > 1), var is an array
};

struct intlit* newIntLit(char* val);

struct fltlit* newFltLit(char* val);

struct strlit* newStrLit(char* str);

struct var* newVar(unsigned int var_type, char* name, int isGlobal, int isParam, unsigned int size);

#endif
