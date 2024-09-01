#pragma once

#include <iostream>

#include "TypeDefs.h"

using namespace std;

class Writer
{
public:
	Writer( ostream &ostr )
	: ostr_( ostr ), pos_( 0 )
	{
	}

	void align()
	{
		if ( pos_ & 1 )
		{
			ostr_ << '\x00';
			++pos_;
		}
	}

	void writebyte( byte b )
	{
		ostr_ << b;
		++pos_;
	}

	void writeword( word w )
	{
		align();
		writebyte( w & 0xFF );
		writebyte( w >> 8 );
	}

	void writedword( dword dw )
	{
		writeword( dw & 0xFFFF );
		writeword( dw >> 16);
	}

	void writedirent( word tag, word type, dword count, dword value )
	{
		writeword( tag );
		writeword( type );
		writedword( count );
		writedword( value );
	}

	void fill( int pos )
	{
		while ( pos_ < pos )
			writebyte( 0 );
	}

	void writestr( const char *str )
	{
		align();
		for (;;)
		{
			char c = *str++;
			writebyte( c );
			if ( !c )
				break;
		}
	}

	size_t pos()
	{
		return pos_;
	}

	void seek( size_t pos )
	{
		ostr_.seekp( pos );
		pos_ = pos;
	}

private:
	ostream &ostr_;
	size_t pos_;
};


