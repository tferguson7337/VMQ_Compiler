# VMQ_Compiler - By Thomas Ferguson
A C++ to VMQ Compiler

### Background Info
This source code is for a Flex/Yacc based compiler, written in C, that will compile a small and basic subset of the C++ language into a virtual assembly-esque code called VMQ, which can be run via the VMQ interpreter.  The VMQ interpreter was written by CSU Stanislaus' Professor Emeritus Raymond L. Zarling, Ph.D. - see Project_Info/VMforQuadsDocumentation_1.pdf and VMQ_src for more info.

This is based entirely off of a school group project at CSU Stanislaus, of which I was the primary contributor.  
The original version that was turned in can be found here: https://github.com/Ssuarez0/CS4300-Group-Projects.  
The final version of the group project was largely unfinished, very buggy, and incredibly messy.

This version is entirely written by me, from the ground up.  My goal is to finish the entire project, and keep the code correct and looking presentable.  
The code is being developed on and for Linux (ubuntu 17.04), the Makefile works based off of this constraint.  However, the source code for the compiler *should* be platform independent, assuming that your platform has access to lex/flex and yacc/bison and whatever libraries they are dependent on.

### What Is This? - A Quick Synopsis
The program will accept a small subset of C++-ish programs and output a plain-text ASCII file that, when run through the VMQ interpreter, results in the equivalent output the C++-ish program would generate.  This VMQ output file is written in a simple assembly-like language; if you've ever played TIS-100, it's like a somewhat more capable version of that game's assembly language.  View the project info pdf on VMQ for more information.

### How To Use It
If you're running from a Linux-based system, you can compile the compiler and VMQ interpreter using the Makefiles provided, which should work (though small edits may be needed).  The compiler (named 'cVMQ' by default) will accept a simple C++-ish source file via command line input; the meaning of "simple C++-ish" is explained below.  Assuming the file is formatted correctly, the compiler will generate an output file matching the name of the C++ file, but with a .q "extension," rather than a .cpp extension (e.g., <file_name>.cpp -> <file_name>.q).  This output file can then be used with the VMQ interpreter, input via command line.

### "C++-ish"?
Yes.  The compiler will accept a small and basic subset of C++-like programs, with a number of caveats.

#### Here's what it supports:
+ int and float data types
  + variables
  + floats
  + int/float-returning functions that take int/float arguments
+ Simple and nested control structures
  + While loops
  + If/else statements
+ Simple I/O
  + Output to stdout using int, float, and literal string values
    + Uses cout, the stream insertion operator (<<), and endl.
    + Escape sequences should work as intended.
  + Input from stdin for int/float variables/array elements
    + Uses cin and the stream extraction operator (>>).
+ Standard math operations
  + Addition (+), subtraction (-)
  + Multiplication (*), division (/), modulo (%)
  + Promotion (i.e., implicit casting)
+ Standard logical operations
  + AND (&&)
  + OR (||)
  + NOT (!)
+ Standard relational operations
  + Equivalence (==), less than (<), greater than (>)
  + Nonequivalence (!=), less than or equal to (<=), greater than or equal to (>=)
+ Assignment
  + Standard assignment (=)
  + Add-assign (+=), sub-assign (-=)
+ Recursion
+ Scope levels
+ Implicit casts
  + int -> float
  + float -> int

#### At the time of writing this, here's what it *does not* support, *including, but not limited to*:
- Declaring variables after function definitions/statements
  - Due to how the target language syntax is defined (see Project_Info/Target-Language-Syntax-2.pdf for more detail), **all* variable definitions in global space must occur prior to any function definition.  Any variable in local scope must be declared prior to any statements within the function.
- Incrementation/decrementation operators (++, --)
  - Equivalent statements (e.g., var = var + 1; or var += 1;) still work, of course.
- If statements with no accompanying else
  - In theory, there *may* be a work-around here where you can simply leave the else block empty and it will accept it, but I haven't tested this.
    - The target language syntax pdf shows that the grammar for if/else accepts a **statement** after an **else**, but a statement can be a compound statement, which can be an empty block (curly braces that enclose no statements).
- Include statements
  - Include statements are simply treated as comments - they are ignored.
    - I/O capabilities are inherently "cherry-picked" from **iostream**, but the compiler itself makes no use of any include files.
  - In fact, any preprocessor statement should be ignored.
- Classes, namespaces, structs, and other more advanced scoping/data structures are unsupported.
  - cin, cout, and endl are accepted without applying the "std::" prefix or a "using namespace std;" statement.
- Non-logical/relational operations in boolean expressions
  - If/else and while loop conditionals cannot contain anything other than some valid boolean expression, as defined in Project_Info/Target-Language-Syntax-2.pdf.
  - Things such as while(x -= 1) or if(x) will not be accepted.
    - Assignment/math operators and variables do not "return" a value, as is typically seen in C/C++ programming.
- Explicit casting
  - As noted above, implicit casts are still supported. A user can easily write their own functions that returns an implicitly cast value (e.g., int castToInt(float f); or float castToFloat(int i);).
