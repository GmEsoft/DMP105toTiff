#pragma once

#include <string>

#include "TypeDefs.h"

using namespace std;

class PackBits
{
public:
	string unpack( const string &str )
	{
		string ret;
		int count = 0;

		for ( int i=0; i<str.size(); ++i )
		{
			signed char c = str[i];

			if ( !count )
			{
				count = c == -128 ? 0 : c+1;
			}
			else if ( count > 0 )
			{
				ret += c;
				--count;
			}
			else
			{
				for ( int j=0; j<2-count; ++j )
					ret += c;
				count = 0;
			}
		}
		return ret;
	}


	string pack( const byte *buf, size_t bufsize, int p, int bytecount )
	{
		string ret;
		byte c, last;
		int r, n, n0, p0;

		p0 = p;
		r = 0;
		n = 0;

		for ( int i=0; i<bytecount; ++i )
		{
			//	n	r	:		A	B	C		D	D	D	D	E	F	G
			//	0	0	:
			//	1	1	:	1<	A
			//	2	1	:	2<	A	B
			//	3	1	:	3<	A	B	C
			//	4	1	:	4<	A	B	C		D
			//	5	2	:	5<	A	B	C		D	D
			//	0	3	:	3	A	B	C	3<	D
			//	0	4	:	3	A	B	C	4<	D
			//	1	1	:	3	A	B	C	4	D				1<	E
			//	2	1	:	3	A	B	C	4	D				2<	E	F
			//	3	1	:	3	A	B	C	4	D				3<	E	F	G

			c = p < bufsize ? buf[p++] : 0;
			if ( r < 2 || c != last )
			{
				if ( n == 0 )
				{
					n0 = ret.size();
					ret += char( 0 );
				}
				++n;
				++r;
				ret[n0] = n-1;
				ret += char( c );
				if ( c != last )
					r = 1;
			}
			else if ( r == 2 )	// 3rd repeated char
			{
				if ( r == n )
				{
					ret = ret.substr( 0, ret.size() - 1 );
				}
				else
				{
					ret[n0] -= 2;
				}
				n0 = ret.size() - 2;
				n = 0;
				++r;
				ret[n0] = 1 - r;
			}
			else
			{
				++r;
				ret[n0] = 1 - r;
			}

			if ( r == 128 || n == 128 )
			{
				r = n = 0;
			}

			last = c;

	#if 0
			dump( cerr, ret );
	#endif
		}
		return ret;
	}
};

