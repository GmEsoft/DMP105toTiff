#pragma once

#include <ostream>
#include <iomanip>

using namespace std;

class HexDump
{
public:
	HexDump( ostream &p_ostr )
	: ostr( p_ostr )
	{}

	void dump( const string &str )
	{
		size_t i,j;
		for ( i=0; i<str.size(); i+=16 )
		{
			ostr << hex << uppercase << setfill('0');

			ostr << setw(6) << i << ":";
			for ( j=i; j<i+16 && j<str.size(); ++j )
			{
				if ( !(j&3) )
					ostr << " ";
				ostr << setw(2) << ( short(str[j]) & 0xFF ) << " ";
			}

			for ( ; j<i+16 ; ++j )
			{
				if ( !(j&3) )
					ostr << " ";
				ostr << "   ";
			}

			for ( j=i; j<i+16 && j<str.size(); ++j )
			{
				ostr << dec << char( ( str[j] < ' ' ) ? '.' : str[j] );
			}

			ostr << endl;
		}
		ostr << endl;
	}

private:
	ostream &ostr;
};
