// vqm.cpp
// Virtual Quadruple Machine for Compiler Theory Class

// Copyright 2002 Raymond L. Zarling

#define VERSION "2.04"

// Note: To compile on a machine that doesn't have <sstream> but
// does have <strstream>, #define DOES_NOT_HAVE_SSTREAM

#include <iostream>
#include <fstream>

#ifdef DOES_NOT_HAVE_SSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include <stdexcept>
#include <cctype>
#include <cerrno>
#include <vector>
#include <climits>

#include "storage.h"
#include "quad.h"

using namespace std;

#ifdef __AMIGA__
static char *VersionString = "$VER: " VERSION "(" __DATE__ ")";
#else
static char *VersionString = VERSION " (" __DATE__ ")";
#endif

static char *Copyright = "Copyright 2002 Raymond L. Zarling";

// Error severity levels
#define ERR_WARN 5
#define ERR_ERROR 10
#define ERR_FATAL 20

// The emulated memory (data memory)
storage_type mem ( 0x7ffc ); // Largest possible multiple of 4

// List of quads (program memory)
vector<quad_type> qlist;

// Error level
int errflag = 0;

// Routines to read a quad file and initialize program and data memory
class qfreader
{
public:
	qfreader ( istream &is, storage_type &mem, vector<quad_type> &qlist )
		: m_errorlevel(0), m_lineno(0), m_is ( is ),
		  m_mem ( mem ), m_qlist ( qlist ) {}
	int go ( void );

private:
	void posterror ( int level, const string &msg ) const;
	qop parse_adr ( const string &s, bool dst, bool flt ) const;
	qop parse_short ( const string &s ) const;
	qop parse_float ( const string &s ) const;

	mutable int m_errorlevel;
	unsigned m_lineno;
	istream &m_is;
	string m_line;	// current source line
	storage_type &m_mem;
	vector<quad_type> &m_qlist;
};

// Routines to interpret a program execution
class interpreter
{
public:
	interpreter ( storage_type &mem, const vector<quad_type> &qlist )
		: m_mem(mem), m_qlist(qlist), m_tracing(false) {}
	int go ( void );

private:
	void posterror ( int level, const string &msg ) const;
	void traceresult ( adr_type res_adr, char res_type );

	storage_type &m_mem;
	const vector<quad_type> &m_qlist;
	adr_type m_pc;	// current program counter
	adr_type m_cur_pc; // pc of current instruction, even after ++m_pc
	adr_type m_gsize; // size of global data area
	bool m_tracing;
	mutable int m_errorlevel;
};

int main ( int argc, char *argv[] )
{
	// Identify
	cerr << "Virtual Quadrule Machine version " << VERSION
		<< " (" << __DATE__ << ")" << endl;
	cerr << Copyright << endl;

	// Read the quad file
	cerr << "Reading quads" << endl;
	switch ( argc )
	{
	case 1:
		{
			qfreader loader ( cin, mem, qlist );
			errflag = loader.go();
		}
		break;
	case 2:
		{
			ifstream qf ( argv[1] );
			if ( !qf )
			{
				// If the file didn't open, errno has information about
				// what went wrong
				cerr << "Can't open file " << argv[1] << ": "
					<< strerror(errno) << endl;
				exit ( 10 );
			}
			qfreader loader ( qf, mem, qlist );
			errflag = loader.go();
			qf.close();
		}
		break;
	default:
		cerr << "Usage: " << argv[0] << " <quadfile>" << endl;
		exit ( 10 );
	}

	// Diagnostic dump of quads
//	for ( int i = 0; i < qlist.size(); ++i )
//		cout << i << ": " << qlist[i] << endl;

	// Don't run if errors in the quad source
	if (errflag > ERR_WARN)
	{
		cerr << "Execution not attempted due to errors in quad file" << endl;
		exit ( errflag );
	}

	cerr << "Running..." << endl;
	interpreter machine ( mem, qlist );
	machine.go();
	return 0;
}

int qfreader::go ( void )
{
	bool datasection = true; /* true while reading static data */

	// line always has at least a '\n' character
	getline ( m_is, m_line );
	for ( ; m_is; getline ( m_is, m_line ) )
	{
		m_line += '\n';
		m_lineno++;

#ifdef DOES_NOT_HAVE_SSTREAM
		istrstream ss(m_line.c_str());
#else
		istringstream ss(m_line);
#endif
		if ( datasection )
		{	/* use the line contents to initialize some data storage */
			if ( !isdigit ( m_line[0] ) ) /* end of data section? */
			{
				datasection = false;
				// execution will continue with decoding quads below
			}
			else // not yet end of datasection
			{
				// Get address to load the constant
				adr_type a;
				ss >> a >> ws;
				// Discover the type of the constant
				// Get a pointer to the value
				string::size_type vptr;
				string::size_type space = m_line.find_first_of(" \t");
				if ( space == string::npos ||
					(vptr = m_line.find_first_not_of ( " \t", space ))
						== string::npos )
				{
					posterror ( ERR_ERROR, "No Initialization Value Found" );
					continue;
				}

				// interpret type of constant, set memory
				switch ( m_line[vptr] )
				{
				case '\"':	// string constant
					try
					{
						string::size_type end;
						end = m_line.find('"', vptr+1);
						if ( end == string::npos )
						{
							posterror ( ERR_WARN, "Unterminated string" );
							end = m_line.length() - 1;
						}
						m_mem.Set( a,
							m_line.substr(vptr+1, end-vptr-1).c_str() );
					}
					catch ( runtime_error &e )
					{
						posterror ( ERR_ERROR, e.what() );
					}
					break;

				case '0': case '1': case '2': case '3': case '4': // numeric
				case '5': case '6': case '7': case '8': case '9':
				case '+': case '-': case '.':
					try
					{
						string::size_type x
							= m_line.find_first_of ( ". \t\n", vptr );
						if ( m_line[x] == '.' )
						{
							float f;
							ss >> f;
							m_mem.Set ( a, f );
						}
						else
						{
							short i;
							ss >> i;
							m_mem.Set ( a, i );
						}
					}
					catch ( runtime_error &e )
					{
						posterror ( ERR_ERROR, e.what() );
					}
					break;

				default:
					posterror ( ERR_ERROR, "Unrecognized data type" );
				} // switch
				continue; // Get the next line
			} // end "else" clause (not end of datasection)
		} // if datasection

		// Control continues here if we are no longer in datasection, or
		// if the datasection code discovered the code section.
		// ss has been initialized, but not yet read from.

		// We cannot handle more than SHRT_MAX quads, because
		// signed short ints are used to address them.
		if ( m_qlist.size() >= SHRT_MAX )
			posterror ( ERR_FATAL, "Too many quads" );

		// Parse the line and fill in a quad structure
		string::size_type i = 0;
		bool traceon = false, traceoff = false, dump = false;
		char ch; // a dummy, for emptying the stringstream

		// Look for debugging flags
		if ( m_line[i] == 'x' )
		{
			++i; traceon = true; ss >> ch;
		}
		if ( m_line[i] == 'X' )
		{
			++i; traceoff = true; ss >> ch;
		}
		if ( m_line[i] == '@' )
		{
			++i; dump = true; ss >> ch;
		}

		char sop;
		string sop1, sop2, sop3; // string representations of quad bits
		const bool f = false, t = true; // notational convenience
		bool r = isupper(m_line[i]); // is current operand a "real"; i.e. float

		switch ( m_line[i] )
		{
		// 3 address quads
		case 'a': case 'A': case 's': case 'S': case 'm': case 'M':
		case 'd': case 'D': case 'r': case '|': case '&':
			ss >> sop >> sop1 >> sop2 >> sop3;
			m_qlist.push_back ( quad_type( sop, parse_adr(sop1,f,r),
				parse_adr(sop2,f,r), parse_adr(sop3,t,r) ) );
			break;
		case 'l': case 'L': case 'g': case 'G': case 'e': case 'E':
			ss >> sop >> sop1 >> sop2 >> sop3;
			m_qlist.push_back ( quad_type( sop, parse_adr(sop1,f,r),
				parse_adr(sop2,f,r), parse_short(sop3) ) );
			break;
		// 2 address quads
		case 'i': case 'I': case '=': case 'F': case 'f': case '~':
		case 'n': case 'N':
		{
			bool sr = r; // is source operand real (float)?
			if ( toupper(m_line[i]) == 'F' ) sr = !r;
			ss >> sop >> sop1 >> sop2;
			m_qlist.push_back
				( quad_type( sop, parse_adr(sop1,f,sr), parse_adr(sop2,t,r) ) );
		}
			break;
		// Control Transfer with address, Label
		case 'c':
			ss >> sop >> sop1 >> sop2;
			m_qlist.push_back
				( quad_type( sop, parse_adr(sop1,f,f), parse_short(sop2) ) );
			break;
		// 1 address quads
		case 'p': case 'P':
			ss >> sop >> sop1;
			m_qlist.push_back ( quad_type( sop, parse_adr(sop1,f,r) ) );
			break;
		// 2 Label  or integer literal quads
		case '$':
			ss >> sop >> sop1 >> sop2;
			m_qlist.push_back
				( quad_type( sop, parse_short(sop1), parse_short(sop2) ) );
			break;
		// 1 Label  or 1 integer literal quads
		case 'j': case '#': case '^':
			ss >> sop >> sop1;
			m_qlist.push_back ( quad_type( sop, parse_short(sop1) ) );
			break;
		// No operands
		case '/': case 'h': case ';':
			ss >> sop;
			m_qlist.push_back ( quad_type(sop) );
			break;
		default:
			{
				string msg = "Invalid operation: ";
				msg += m_line[i];
				posterror ( ERR_ERROR, msg );
			}
		} // end switch

		// Record diagnostic flags
		if ( m_errorlevel <= ERR_WARN )
		{
			m_qlist.back().TraceOn ( traceon );
			m_qlist.back().TraceOff ( traceoff );
			m_qlist.back().DumpOn ( dump );
		}
	} // for m_is

	return m_errorlevel;
}

// Form a string version of an operand into a qop structure, interpreting
// it as (1) an address, (2) a short, or (3) a float (separate functions
// for three separate interpretations).

// An "address" operand may actually be a different type, if specified
// in immediate mode.  If dst is true, disallow immediate mode addressing
// because the operand is a destination.  If flt is true, require the
// operand to be a float if given in immediate mode.
qop qfreader::parse_adr ( const string &s, bool dst, bool flt ) const
{
	char code = ' '; // addressing mode
	char utype = 'a'; // type of operand: 's', 'a', or 'f'
	string::size_type i = 0;

	if ( s[i] == '@' || s[i] == '#' )
	{
		code = s[i++];
		if ( code == '#' ) // find type of immediate operand
		{
			if ( s[i] == '-' ) utype = 's';
			string::size_type x = s.find_first_of ( ". \t\n", i );
			if ( s[x] == '.' ) utype = 'f';
		}
	}
	if ( s[i] == '/' )
	{
		if ( code == '@' ) code = 'N';
		else if ( code == '#' ) code = 'M';
		else code = '_';
		++i;
	}

	// integrity checks
	if ( code == '#' || code == 'M' )
	{
		if ( dst )
		{
			posterror ( ERR_ERROR, "Destination operand cannot be immediate" );
			return qop ( short(0) );
		}
		if ( flt && utype != 'f' )
		{
			posterror ( ERR_ERROR, "Instruction requires float operand" );
			return qop ( float(0.0) );
		}
		if ( !flt && utype == 'f' )
		{
			posterror ( ERR_ERROR, "Instruction requires int operand" );
			return qop ( short(0) );
		}
	}

#ifdef DOES_NOT_HAVE_SSTREAM
	istrstream ss ( s.substr(i,s.length()-i).c_str() );
#else
	istringstream ss ( s.substr(i,s.length()-i) );
#endif
	switch ( utype )
	{
	case 's':
		{
			short val;
			ss >> val;
			return qop(val, code, 's');
		}
	case 'a':
		{
			// Some addresses are negative offsets
			int val;
			ss >> val;
			return qop(adr_type(val), code, 'a');
		}
	case 'f':
		{
			float val;
			ss >> val;
			return qop(val, code, 'f');
		}
	}
}

qop qfreader::parse_short ( const string &s ) const
{
	if ( !isdigit(s[0]) && s[0] != '-' && s[0] != '+' )
	{
		posterror ( ERR_ERROR, "Illegal Operand" );
		return qop( short(0) );
	}
#ifdef DOES_NOT_HAVE_SSTREAM
	istrstream ss(s.c_str());
#else
	istringstream ss(s);
#endif
	short val;
	ss >> val;
	return qop(val);
}

qop qfreader::parse_float ( const string &s ) const
{
	if ( !isdigit(s[0]) && s[0] != '-' && s[0] != '+' )
	{
		posterror ( ERR_ERROR, "Illegal Operand" );
		return qop ( float(0.0) );
	}
#ifdef DOES_NOT_HAVE_SSTREAM
	istrstream ss(s.c_str());
#else
	istringstream ss(s);
#endif
	float val;
	ss >> val;
	return qop(val);
}

// Post a "loading-time" error
// Note that errors of level FATAL cause immediate program termination
void qfreader::posterror ( int level, const string &msg ) const
{
	string severity = "??? ";
	switch ( level )
	{
	case ERR_WARN: severity = "Warning: "; break;
	case ERR_ERROR: severity = "Error: "; break;
	case ERR_FATAL: severity = "Fatal Error!: "; break;
	}

	cerr << "line " << m_lineno << ": " << m_line; // m_line has '\n'
	cerr << severity << msg << endl;
	if ( level > m_errorlevel )
		m_errorlevel = level;
	if ( level >= ERR_FATAL ) exit ( m_errorlevel );
}

// Run the simulated machine
int interpreter::go ( void )
{
	m_errorlevel = 0;
	m_tracing = false;
	size_t nquads = m_qlist.size();
	bool running = false; // record when a '$' has been executed
	bool stop = false; // respond to 'h' quad

	// Get start address
	if ( nquads == 0 ) return m_errorlevel;
	if ( m_qlist[0].op() != '$' )
	{
		posterror ( ERR_ERROR, "First quad must be '$'" );
		return m_errorlevel;
	}
	m_pc = 0;

	// Main Interpretive Loop
	char cur_op = m_qlist[m_pc].op();
	qop op1, op2, op3; // The up-to-3 operands
	adr_type res_adr; // If there's a memory result, its absolute address
	char res_type; // If there's a memory result, 'a', 's' or 'f'
	try
	{
		while ( !stop )
		{
			m_cur_pc = m_pc;

			// Act on diagnostic flags
			if ( m_qlist[m_pc].tron() ) m_tracing = true;
			if ( m_qlist[m_pc].troff() ) m_tracing = false;
			if ( m_qlist[m_pc].dump() )
			{
				cout << "Global Data Area:" << endl;
				m_mem.Dump ( 0, m_gsize-1 );
				cout << "\nRuntime Stack Area:" << endl;
				m_mem.Dump ( m_mem.STop(), m_mem.Size()-1, true );
			}

			if ( m_tracing ) cout << setw(4) << m_pc << ": "
				<< m_qlist[m_pc]; // Do endl later...

			// Do the operation
			switch ( cur_op )
			{
			// 3 address quads
			case 'a': case 'A': case 's': case 'S': case 'm': case 'M':
			case 'd': case 'D': case 'r': case '|': case '&':
				op1 = m_qlist[m_pc].op1();
				op2 = m_qlist[m_pc].op2();
				op3 = m_qlist[m_pc].op3();
				res_adr = op3.lval(m_mem);
				m_pc++;
				switch ( cur_op )
				{
				case 'a': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) + op2.sval(m_mem)) );
					res_type = 's';
					break;
				case 'A': m_mem.Set ( res_adr,
						float(op1.fval(m_mem) + op2.fval(m_mem)) );
					res_type = 'f';
					break;
				case 's': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) - op2.sval(m_mem)) );
					res_type = 's';
					break;
				case 'S': m_mem.Set ( res_adr,
						float(op1.fval(m_mem) - op2.fval(m_mem)) );
					res_type = 'f';
					break;
				case 'm': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) * op2.sval(m_mem)) );
					res_type = 's';
					break;
				case 'M': m_mem.Set ( res_adr,
						float(op1.fval(m_mem) * op2.fval(m_mem)) );
					res_type = 'f';
					break;
				case 'd': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) / op2.sval(m_mem)) );
					res_type = 's';
					break;
				case 'D': m_mem.Set ( res_adr,
						float(op1.fval(m_mem) / op2.fval(m_mem)) );
					res_type = 'f';
					break;
				case 'r': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) % op2.sval(m_mem)) );
					res_type = 's';
					break;
				case '|': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) | op2.sval(m_mem)) );
					res_type = 's';
					break;
				case '&': m_mem.Set ( res_adr,
						short(op1.sval(m_mem) & op2.sval(m_mem)) );
					res_type = 's';
					break;
				}
				if ( m_tracing ) traceresult ( res_adr, res_type );
				break;

			// Quads with 2 addresses and a label
			case 'l': case 'L': case 'g': case 'G': case 'e': case 'E':
				op1 = m_qlist[m_pc].op1();
				op2 = m_qlist[m_pc].op2();
				op3 = m_qlist[m_pc].op3();
				m_pc++; // in case branch is not taken
				{
					bool take = false; // take the branch?
					switch ( cur_op )
					{
					case 'l':
						take = (op1.sval(m_mem) < op2.sval(m_mem));
						break;
					case 'L':
						take = (op1.fval(m_mem) < op2.fval(m_mem));
						break;
					case 'g':
						take = (op1.sval(m_mem) > op2.sval(m_mem));
						break;
					case 'G':
						take = (op1.fval(m_mem) > op2.fval(m_mem));
						break;
					case 'e':
						take = (op1.sval(m_mem) == op2.sval(m_mem));
						break;
					case 'E':
						take = (op1.fval(m_mem) == op2.fval(m_mem));
						break;
					}
					if ( take )
					{
						m_pc = op3.ival(m_mem);
						if ( m_tracing && cur_op != 'c' )
							cout << " --> branch to " << m_pc;
					}
					else
					{
						if ( m_tracing && cur_op != 'c' )
							cout << " --> branch not taken";
					}
				}
				break;

			// 2 address quads
			case 'i': case 'I': case '=': case 'F': case 'f':
			case '~': case 'n': case 'N':
				op1 = m_qlist[m_pc].op1();
				op2 = m_qlist[m_pc].op2();
				res_adr = op2.lval(m_mem);
				m_pc++;
				switch ( cur_op )
				{
				case 'i': m_mem.Set ( res_adr, op1.sval(m_mem) );
					res_type = 's';
					break;
				case 'I': m_mem.Set ( res_adr, op1.fval(m_mem) );
					res_type = 'f';
					break;
				case '=': m_mem.Set ( res_adr, char(op1.cval(m_mem)) );
					res_type = 'c';
					break;
				case 'F': m_mem.Set ( res_adr, float( op1.sval(m_mem) ) );
					res_type = 'f';
					break;
				case 'f': m_mem.Set ( res_adr, short( op1.fval(m_mem) ) );
					res_type = 's';
					break;
				case '~': m_mem.Set ( res_adr, short( ~op1.sval(m_mem) ) );
					res_type = 's';
					break;
				case 'n': m_mem.Set ( res_adr, short ( -op1.sval(m_mem) ) );
					res_type = 's';
					break;
				case 'N': m_mem.Set ( res_adr, float ( -op1.fval(m_mem) ) );
					res_type = 'f';
					break;
				} // end switch
				if ( m_tracing ) traceresult ( res_adr, res_type );
				break;

			// Function call with address, Label
			case 'c':
				op1 = m_qlist[m_pc].op1();
				op2 = m_qlist[m_pc].op2();
				m_pc++;
				// Note: if m_tracing, the endl is handled here
				// so we aren't in the middle of a tracing I/O
				// during pseudo-calls to virtual I/O operations
				if ( m_tracing )
					cout << " --> Call function at " << op2.ival(m_mem) << endl;
				if ( op2.ival(m_mem) >= 0 ) // Real function call
				{
					// push result address and return address
					m_mem.Push ( op1.aval(m_mem) );
					m_mem.Push ( m_pc );
					m_pc = op2.ival(m_mem);
				}
				else // Pseudo-function call
				{
					// pseudo-calls to do I/O--the variable to be
					// set or printed is on top of the stack.
					adr_type arg = m_mem.Adr ( m_mem.STop() );
					switch ( op2.ival(m_mem) )
					{
					case -1: // Read int
						{
							short x;
							cin >> x;
							m_mem.Set ( arg, x );
						}
						break;
					case -2: // Read float
						{
							float x;
							cin >> x;
							m_mem.Set ( arg, x );
						}
						break;
					case -3: // Read character line
						{
							string x;
							getline ( cin, x );
							x += '\n';
							m_mem.Set ( arg, x.c_str(), x.length()+1 );
						}
						break;
					case -9: // Write int
						{
							short x = m_mem.Short(arg);
							cout << x;
						}
						break;
					case -10: // Write float
						{
							float x = m_mem.Float(arg);
							cout << x;
						}
						break;
					case -11: // Write string
						{
							char *x = m_mem.Str(arg);
							cout << x;
						}
						break;
					default:
						posterror ( ERR_ERROR,
							"Unrecognized pseudo-quad number: STOP" );
						return m_errorlevel;
					} // end switch
				} // end Pseudo-function call
				break;

			// 1 address quads
			case 'p': case 'P':
				op1 = m_qlist[m_pc].op1();
				m_pc++;
				// Check for stack overflow
				if ( m_mem.STop() - (cur_op=='p'? 2: 4) < m_gsize )
					posterror ( ERR_FATAL, "Stack Overflow" );
				if ( cur_op == 'p' )
					m_mem.Push ( op1.aval(m_mem) );
				else
					m_mem.Push ( op1.fval(m_mem) );
				if ( m_tracing )
				{
					ios::fmtflags fmt = cout.flags(); // base, internal
					int oldfill = cout.fill('0');
					cout.setf(ios::hex, ios::basefield);
					cout.setf(ios::internal, ios::adjustfield);
					adr_type i;
					cout << " --> stack now";
					for ( i = m_mem.STop();
						i <= m_mem.DLink() && i < m_mem.STop()+16; i+=2 )
					{
						cout << " " << setw(4) << m_mem.Adr(i);
					}
					if ( i <= m_mem.DLink() ) cout << "...";
					cout.fill(oldfill);
					cout.flags(fmt);
				}
				break;

			// 2 Label  or integer literal quads
			case '$':
				if ( running )
				{
					posterror ( ERR_ERROR,
						"'$' Quad may only be executed once: STOP" );
					return m_errorlevel;
				}
				running = true;
				op1 = m_qlist[m_pc].op1();
				op2 = m_qlist[m_pc].op2();

				m_pc = op1.ival(m_mem);
				m_gsize = op2.ival(m_mem);
				if ( m_tracing ) cout << " --> " << m_pc;
				break;

			// 1 Label  or 1 integer literal quads
			case 'j': case '#': case '^':
				op1 = m_qlist[m_pc].op1();
				if ( cur_op == 'j' ) // unconditional jump
				{
					m_pc = op1.ival(m_mem);
					if ( m_tracing ) cout << " --> " << m_pc;
					break;
				}
				m_pc++;
				if ( cur_op == '#' ) // create stack frame
				{
					m_mem.Link ( op1.ival(m_mem) );
					break;
				}
				if ( cur_op == '^' ) // pop runtime stack
				{
					size_t n = op1.ival(m_mem);
					if ( n & 0x0001 )
					{
						posterror ( ERR_ERROR,
							"Must pop an even number of bytes: STOP" );
						return m_errorlevel;
					}
					m_mem.Pop ( n );
					if ( m_tracing )
					{
						const ios::fmtflags fmt = cout.flags();
						const int oldfill = cout.fill('0');
						cout.setf(ios::hex, ios::basefield);
						cout.setf(ios::internal, ios::adjustfield);
						cout.setf(ios::showbase);
						cout << "  --> Stack Top (" << setw(6) << m_mem.STop()
							<< ") = " << setw(6) << m_mem.Adr(m_mem.STop());
						cout.fill(oldfill);
						cout.flags(fmt);
					}
				}
				break;

			// No operands
			case '/':
				m_mem.UnLink();
				m_pc = m_mem.Pop_Adr();
				(void) m_mem.Pop_Adr(); // Pop adr of return value
				if ( m_tracing ) cout << " --> " << m_pc;
				break;
			case 'h':
				stop = true;
				break;
			case ';':
			m_pc++;
				break;

			default:
				posterror ( ERR_ERROR, "Unrecognized opcode" );
				return m_errorlevel;
			}

			if ( m_tracing && cur_op != 'c' ) cout << endl;

			cur_op = m_qlist[m_pc].op();
		} // end Interpretive Loop
	}
	catch ( runtime_error &e )
	{
		posterror ( ERR_ERROR, e.what() );
	}

	return m_errorlevel;
}

// Output the result of a memory operation for tracing
void interpreter::traceresult ( adr_type res_adr, char res_type )
{
	const ios::fmtflags fmt = cout.flags();
	const int oldfill = cout.fill('0');
	cout.setf(ios::showbase);
	cout.setf(ios::internal, ios::adjustfield);
	cout << hex;
	cout << " --> (" << setw(6) << res_adr << ") = ";

	switch ( res_type )
	{
	case 'a': cout << setw(6) << m_mem.Adr( res_adr ); break;
	case 's': cout << setw(6) << m_mem.Short(res_adr)
		<< dec << " ( = " << m_mem.Short( res_adr ) << " )";
		break;
	case 'c': cout << /*setw(4) <<*/ (unsigned short) m_mem.Char(res_adr);
		if ( isprint ( m_mem.Char(res_adr) ) )
			cout << " ( = " << m_mem.Char( res_adr ) << " )";
		break;
	case 'f': cout << m_mem.Float( res_adr ); break;
	}
	cout.fill(oldfill);
	cout.flags(fmt);
}

// Post a "run-time" error
// Note that errors of level FATAL cause immediate program termination
void interpreter::posterror ( int level, const string &msg ) const
{
	string severity = "??? ";
	switch ( level )
	{
	case ERR_WARN: severity = "Warning: "; break;
	case ERR_ERROR: severity = "Error: "; break;
	case ERR_FATAL: severity = "Fatal Error!: "; break;
	}

	cerr << "\nQuad address " << m_cur_pc << ": " << m_qlist[m_cur_pc] << endl;
	cerr << severity << msg << endl;
	if ( level > m_errorlevel )
		m_errorlevel = level;
	if ( level >= ERR_FATAL ) exit ( m_errorlevel );
}
