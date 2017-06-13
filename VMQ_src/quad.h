// quad.h
// Quadruple representation for the Compiler Theory Class interpreter

// Copyright 2002 Raymond L. Zarling

#ifndef QUAD_H
#define QUAD_H

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "storage.h"

using namespace std;

// A quadruple operand value can be of one of several types
union opval
{
	short s;	// int or quadruple number (label)
	adr_type a;	// data memory address
	float f;	// float

	opval ( void ): a() {}	// There is no unambiguous null value; use 0
	opval ( short sv ): s(sv) {}
	opval ( adr_type av ): a(av) {}
	opval ( float fv ): f(fv) {}
};

// A complete quadrule operand must include an encoding for access type
struct qop
{
	opval val;	// Numeric value
	char vtype;	// From union opval: 's', 'a', or 'f'
	char adrmode;	// absolute: '#' immediate; ' ' normal; '@' indirect
					// base-rel: 'M' immediate; '_' normal; 'N' indirect

	qop ( void ): val(), vtype('a'), adrmode(' ') {}
	qop ( short v, char a = ' ', char vt = 's' )
		: val(v), vtype(vt), adrmode(a) {}
	qop ( adr_type v, char a = ' ', char vt = 'a' )
		: val(v), vtype(vt), adrmode(a) {}
	qop ( float v, char a = ' ', char vt = 'f' )
		: val(v), vtype(vt), adrmode(a) {}
	void set ( short v, char a = ' ', char vt = 's' )
		{ val = v; vtype = vt; adrmode = a; }
	void set ( adr_type v = 0, char a = ' ', char vt = 'a' )
		{ val = v; vtype = vt; adrmode = a; }
	void set ( float v, char a = ' ', char vt = 'f' )
		{ val = v; vtype = vt; adrmode = a; }

	// Functions to compute l- and r-values
	inline adr_type lval ( const storage_type &mem ) throw ( runtime_error )
	{
		switch ( adrmode )
		{
		case '#': case 'M': throw runtime_error ("Illegal use of l-value" );
		case '_': return adr_type ( mem.DLink() + val.s );
		case 'N': return mem.Adr ( adr_type(mem.DLink() + val.s) );
		case ' ': return val.a;
		case '@': return mem.Adr ( val.a );
		}
	}
	// r-values come in several types

	// For integer or label operands
	inline short ival ( const storage_type &mem ) throw ( runtime_error )
	{
		switch ( adrmode )
		{
		case '#': case 'M': case '_': case 'N': case '@':
			throw runtime_error ("Illegal addressing mode for this operand" );
		case ' ': return val.s;
		}
	}

	// for addresses referring to shorts
	short sval ( const storage_type &mem )
	{
		switch ( adrmode )
		{
		case 'M': return short ( mem.DLink() + val.s );
		case '#': return short ( val.s );
		case '_': return mem.Short ( adr_type(mem.DLink() + val.s) );
		case ' ': return mem.Short ( val.a );
		case 'N': return
			mem.Short ( mem.Adr ( adr_type(mem.DLink() + val.s) ) );
		case '@': return mem.Short ( mem.Adr ( val.a ) );
		}
	}

	// for addresses referring to chars
	short cval ( const storage_type &mem )
	{
		switch ( adrmode )
		{
		case 'M': return char ( mem.DLink() + val.s );
		case '#': return char ( val.s );
		case '_': return mem.Char ( adr_type(mem.DLink() + val.s) );
		case ' ': return mem.Char ( val.a );
		case 'N': return
			mem.Char ( mem.Adr ( adr_type(mem.DLink() + val.s) ) );
		case '@': return mem.Char ( mem.Adr ( val.a ) );
		}
	}

	// for addresses referring to floats
	float fval ( const storage_type &mem )
	{
		switch ( adrmode )
		{
		case '#': case 'M': return val.f;
		case '_': return mem.Float ( adr_type(mem.DLink() + val.s) );
		case ' ': return mem.Float ( val.a );
		case 'N': return
			mem.Float ( mem.Adr ( adr_type(mem.DLink() + val.s) ) );
		case '@': return mem.Float ( mem.Adr ( val.a ) );
		}
	}

	// for addresses referring to addresses
	adr_type aval ( const storage_type &mem )
	{
		switch ( adrmode )
		{
		case 'M': return adr_type ( mem.DLink() + val.s );
		case '#': return val.a;
		case '_': return mem.Adr ( adr_type(mem.DLink() + val.s) );
		case ' ': return mem.Adr ( val.a );
		case 'N': return
			mem.Adr ( mem.Adr ( adr_type(mem.DLink() + val.s) ) );
		case '@': return mem.Adr ( mem.Adr ( val.a ) );
		}
	}

	friend ostream & operator << ( ostream &os, const qop &a );
};

class quad_type
{
public:
	quad_type ( char op = '\0', qop o1 = qop(), qop o2 = qop(), qop o3 = qop() )
		: m_op(op), m_o1(o1), m_o2(o2), m_o3(o3),
			m_tron(false), m_troff(false), m_dump(false) {}
	void Set ( char op = '\0', qop o1 = qop(), qop o2 = qop(), qop o3 = qop() )
		{ m_op = op; m_o1 = o1; m_o2 = o2; m_o3 = o3; }
	void TraceOn ( bool set = true ) { m_tron = set; }
	void TraceOff ( bool set = true ) { m_troff = set; }
	void DumpOn ( bool set = true ) { m_dump = set; }

	// Access functions
	char op ( void ) const { return m_op; }
	bool tron ( void ) const { return m_tron; }
	bool troff ( void ) const { return m_troff; }
	bool dump ( void ) const { return m_dump; }
	const qop &op1 ( void ) const { return m_o1; }
	const qop &op2 ( void ) const { return m_o2; }
	const qop &op3 ( void ) const { return m_o3; }

	friend ostream & operator << ( ostream &os, const quad_type &q );

private:
	char m_op;
	bool m_tron, m_troff, m_dump;
	qop m_o1, m_o2, m_o3;
};

ostream & operator << ( ostream &os, const qop &q )
{
	const ios::fmtflags fmt = os.flags();
	const int oldfill = os.fill('0');
	os.setf(ios::hex, ios::basefield);
	os.setf(ios::internal, ios::adjustfield);
	os.setf(ios::showbase);
	string s = "";
	if ( q.adrmode != ' ' )
	{
		s = q.adrmode;
		if ( q.adrmode == 'M' ) s = "#/";
		else if ( q.adrmode == '_' ) s = "/";
		else if ( q.adrmode == 'N' ) s = "@/";
	}
	if ( q.vtype == 'f' )
		os << s << setfill(' ') << dec << q.val.f;
	else if ( q.vtype == 's' )
		os << s << setfill(' ') << dec << q.val.s;
	else
		os << s << setw(6) << q.val.a;
	os.fill(oldfill);
	os.flags(fmt);
	return os;
}

ostream & operator << ( ostream &os, const quad_type &q )
{
	if ( q.m_tron ) os << 'x';
	if ( q.m_troff ) os << 'X';
	if ( q.m_dump ) os << '@';
	os << '(';
	if ( q.m_op )
	{
		const ios::fmtflags fmt = os.flags();
		os <<q.m_op;
		switch ( q.m_op )
		{
		// 3 address quads
		case 'a': case 'A': case 's': case 'S': case 'm': case 'M':
		case 'd': case 'D': case 'r': case '|': case '&':
			os	<< ", " << q.m_o1
				<< ", " << q.m_o2
				<< ", " << q.m_o3;
			break;
		case 'l': case 'L': case 'g': case 'G': case 'e': case 'E':
			os	<< ", " << q.m_o1
				<< ", " << q.m_o2
				<< ", " << q.m_o3;
			break;
		// 2 address quads
		case 'i': case 'I': case 'F': case '=': case 'f':
		case '~': case 'n': case 'N':
			os	<< ", " << q.m_o1
				<< ", " << q.m_o2;
			break;
		// Function call with address, Label
		case 'c':
			os	<< ", " << q.m_o1
				<< ", " << q.m_o2;
			break;
		// 1 address quads
		case 'p': case 'P':
			os	<< ", " << q.m_o1;
			break;
		// 2 Label  or integer literal quads
		case '$':
			os << ", " << q.m_o1 << ", " << q.m_o2;
			break;
		// 1 Label  or 1 integer literal quads
		case 'j': case '#': case '^':
			os << ", " << q.m_o1;
			break;
		// No operands
		case '/': case 'h': case ';':
			break;
		default: os << "Invalid operation: 0x"
			<< setw(2) << hex << int(q.m_op);
		}
		os.flags ( fmt );
	}
	else // !q.m_op
		os << "uninitialized";
	return os << ")";
}

#endif // QUAD_H
