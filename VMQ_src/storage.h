// storage.h
// Emulated memory for the Compiler Theory Class interpreter

// Copyright 2002 Raymond L. Zarling

#ifndef STORAGE_H
#define STORAGE_H

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstring> // for memcpy
#include <string>
#include <stdexcept>

using namespace std;

typedef unsigned short adr_type;	// Representation of emulated addresses


class storage_type
{
public:
	// Constructor.  Note that size must really be no more than SHRT_MAX,
	// because of the range of adr_type, and because addresses can be
	// represented in base-displacement form where the displacement
	// is negative.
	storage_type ( const size_t size ): m_size(size), m_top(size), m_link(size)
	{
		m_store = new char[m_size];
	};
	~storage_type ()	{ delete[] m_store; }

//		Report
	inline size_t Size ( void ) const { return m_size; };

//		Functions to access data of various types
	inline char Char ( const adr_type adr ) const
		{ return *l_byte ( adr ); };
	inline short Short ( const adr_type adr ) const
		{ return *l_short ( adr ); };
	inline adr_type Adr ( const adr_type adr ) const
		{ return *l_adr ( adr ); };
	inline float Float ( const adr_type adr ) const
		{
			float f;
			memcpy ( &f, l_float ( adr ), sizeof(float) );
			return f;
		}
	inline char *Str ( const adr_type adr )
		{ return l_str ( adr ); };

//		Functions to write to emulated memory
	inline void Set ( const adr_type adr, const char val )
		{ *l_byte(adr) = val; };
	inline void Set ( const adr_type adr, const short val )
		{ *l_short(adr) = val; };
	inline void Set ( const adr_type adr, const adr_type val )
		{ *l_adr(adr) = val; };
	inline void Set ( const adr_type adr, const float val )
		{
			memcpy ( l_float(adr), &val, sizeof(float) );
		};
	// Copy a string verbatim, not interpretting escape sequences
	inline void Set ( const adr_type adr, const char *val, int n )
		{
			memcpy ( l_byte(adr), val, n );
		};
	// Copy a string, interpretting escape sequences
	inline void Set ( const adr_type adr, const char * const val )
		{
			const char *pv = val;
			char *ps = l_str(adr);
			unsigned char c;
			while ( true )
			{
				c = *pv++;
				if ( !c ) { *ps = c; break; } // stop on '\0'
				if ( c == '\\' )
				{
					switch ( c = *pv++ )
					{
					case '\\':
					case 0:  c = '\\'; break; // '\\'
					case 'n': c = '\n'; break; // '\n'
					case 't': c = '\t'; break; // '\t'
					case '0': // '\099'
						{
							unsigned short val = 0;
							while ( (c = *pv++) && isdigit(c) )
								val = 8 * val + (unsigned short)c;
							if (c) pv--; // put back non-digit
							c = val;
						}
						break;
					default: break;
					} // end switch
				}
				*ps++ = c;
			} // end while true
		}

//		Runtime Stack functions
	inline void Push ( const adr_type val )
		{ m_top -= sizeof(adr_type); Set ( m_top, val ); }
	inline void Push ( const short val )
		{ m_top -= sizeof(short); Set ( m_top, val ); }
	inline void Push ( const float val )
		{ m_top -= sizeof(float); Set ( m_top, val ); }
	inline adr_type Pop_Adr ( void )
		{ adr_type result = *l_adr(m_top); m_top += sizeof(adr_type);
			return result; }
	inline short Pop_Short ( void )
		{ short result = *l_short(m_top); m_top += sizeof(short);
			return result; }
	inline float Pop_Float ( void )
		{
			float result;
			memcpy ( &result, l_float(m_top), sizeof(float) );
			m_top += sizeof(float);
			return result;
		}
	inline void Pop ( size_t n ) // Pop and discard n bytes (n even)
		{ m_top += n; }

	// Build a new stack frame
	inline void Link ( const size_t n )
		{ Push ( m_link ); m_link = m_top; m_top -= n; }
	// Unlink--note parameters are left on the stack, to be cleaned
	// up when calling program is unlinked.
	inline void UnLink ( void )
		{ m_top = m_link; m_link = Pop_Adr(); }

	// Access functions: Dynamic Link and Stack Top
	inline adr_type DLink ( void ) const { return m_link; }
	inline adr_type STop ( void ) const { return m_top; }

//		Debug
	// Print contents of memory from adr1 to adr2 inclusive.  If 'stack'
	// is true, print the stack limits and dynamic link register.
	void Dump ( const adr_type adr1, const adr_type adr2, bool stack=false )
		const
	{
		const ios::fmtflags fmt = cout.flags();
		const int oldfill = cout.fill('0');
		int bytecount = 0;
		adr_type dl = m_link; // mark the dynamic links in stack

		cout.setf(ios::hex, ios::basefield);
		cout.setf(ios::internal, ios::adjustfield);
		for (adr_type a=adr1; a <= adr2; a++ )
		{
			if ( bytecount % 16 == 0 )
			{
				cout.setf(ios::showbase);
				if ( bytecount ) cout << "\n";
				cout << setw(6) << a;
				cout.unsetf(ios::showbase);
			}
			else if ( bytecount % 8 == 0 ) cout << "  ";
			if ( (++bytecount) & 1 ) cout << " ";
			if ( a == dl+1 )
			{
				cout << "_"; dl = Adr(dl);
			}
			else cout << " ";
//	Here, we should be able to simply do:
//			cout << setw(2)
//				<< (((unsigned short)m_store[a]) & 0xff);
//	However, g++ 3.2 on Sun produces a program that segmentation faults
//	if we do.  So, here's a work-around:
			{
				unsigned short data = m_store[a];
				data &= 0xff;
				if ( data < 0x10 ) cout << '0';
				cout << data;
			}
		}
		cout << endl;
		cout.setf(ios::showbase);
		if ( stack )
			cout << "Stack: " << setw(6) << m_top << "->" << setw(6) << m_link
				<< endl;
		cout.fill(oldfill);
		cout.flags ( fmt );
	};

private:
//		Function members

//		Functions to access data of various types
	// "byte" may be used to access single elements of a string
	inline char * l_byte ( const adr_type adr ) const
		{ return &m_store[adr]; };
	// "short" is for integers and quad numbers (labels)
	inline short * l_short ( const adr_type adr ) const
		{ check_align(adr, 2); return (short *)&m_store[adr]; };
	// "adr_type" is for addresses (pointers) to the emulated storage
	inline adr_type * l_adr ( const adr_type adr ) const
		{ check_align(adr, 2); return (adr_type *)&m_store[adr]; };
	// "float" is for floating point numbers
	// The address may not be properly aligned for a float on some
	// systems, so memcpy to/from this address to access
	inline void * l_float ( const adr_type adr ) const
		{ check_align(adr, 2); return (void *)&m_store[adr]; };
	// "str" is for character strings, terminated by '\0'
	inline char * l_str ( const adr_type adr )
		{ return &m_store[adr]; }

//		Functions to check proper usage of emulated memory
	void check_align ( const adr_type adr, const int mult ) const
		throw (runtime_error)
	{
		const char name[] = "01234";
		string msg =
			"Unaligned data access, expecting multiple of ";
		msg += name[mult];
		if ( adr & (mult - 1) )
		{
			throw runtime_error(msg);
		}
	}

//		Data members

	size_t m_size; // Emulated memory size in bytes
	char *m_store; // The emulated memory

	// Runtime stack: the stack grows downward from the end of emulated
	// memory.  Stack frame begins with parameter addresses, then return
	// address, then dynamic link, then local storage.
	adr_type m_top;	// points to top element of the runtime stack
	adr_type m_link;	// base of current stack frame (pts to dynamic link)
};

#endif // STORAGE_H
