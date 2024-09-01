#include "HexDump.h"

#include <fstream>
#include "DMP105.h"

const char title[] = "** DMP105toTiff - v0.0.1-alpha - (C) 2024 GmEsoft, All rights reserved. **";

const char help[] =
	"Usage:   DMP105toTiff [options] [-i:InputFile[.txt]] -o:OutputFile[.tif]\n"
	"         -I:inputfile[.txt]   input source file (default: stdin)\n"
	"         -O:outputfile[.tif]  output TIFF file\n"
	"Options: -D   Self-test/Demo page and exit\n"
	"         -T:n Top Lines\n"
	"         -L   Lines per page [72]\n"
	"         -P   Horizontal Pitch (P=Pica ,E=Elite, C=Condensed\n"
	"         -Wn  Dot Weight (1-3)\n"
	"         -C   CR=CR (default: CR=CR+LF)\n"
	"         -Sn  Skip n lines at end of page\n"
	;


using namespace std;


int main( int argc, const char* argp[] )
{
	cerr << title << endl << endl;
	char buf[256];
	const string stab = "\t";

	string infile, outfile, lstfile;

	bool test = false;

	int lines = 72;
	int toplines = 0;
	int skiplines = 0;
	int pitch = 0;
	int weight = 2;
	bool crlf = true;

	for ( int i=1; i<argc; ++i )
	{
		const char *p = argp[i];
		char c = *argp[i];
		if ( c == '-' || c == '/' )
		{
			++p;
			c = *p++;
			switch ( toupper( c ) )
			{
			case 'I':
				if ( *p == ':' )
					++p;
				infile = p;
				break;
			case 'O':
				if ( *p == ':' )
					++p;
				outfile = p;
				break;
			case 'D':
				test = true;
				break;
			case 'T':
				if ( *p == ':' )
					++p;
				toplines = atoi( p );
				break;
			case 'L':
				if ( *p == ':' )
					++p;
				lines = atoi( p );
				break;
			case 'P':
				if ( *p == ':' )
					++p;
				c = toupper( *p );
				pitch
					= c == 'P' ? 0x13
					: c == 'C' ? 0x14
					: c == 'E' ? 0x17
					: 0;
				break;
			case 'S':
				if ( *p == ':' )
					++p;
				skiplines = atoi( p );
				break;
			case 'W':
				if ( *p == ':' )
					++p;
				weight = atoi( p );
				break;
			case 'C':
				crlf = false;
				break;
			case '?':
				cerr << help << endl;
				exit( 0 );
				break;
			}
		}
	}

	if 	( !infile.empty() && infile.find( "." ) == string::npos )
	{
		infile += ".txt";
	}

	if 	( !outfile.empty() && outfile.find( "." ) == string::npos )
	{
		outfile += ".tif";
	}


	ifstream in;

	if ( !infile.empty() )
	{
		in.open( infile.data(), ios::binary );

		if ( !in )
		{
			cerr << "Failed to open input file [" << infile << "]" << endl;
			exit( 1 );
		}
	}

	ofstream out;

	out.open( outfile.data(), ios::binary );

	if ( !out )
	{
		cerr << "Failed to open output file [" << outfile << "]" << endl;
		exit( 1 );
	}

	istream &istr = !infile.empty() ? in : cin;

	DMP105 prn( out, 2100, 3100, 254, lines );

	prn.setPitch( pitch );
	prn.setCrLf( crlf );
	prn.setTopLines( toplines );
	prn.setSkipLines( skiplines );
	prn.setWeight( weight );

	if ( test )
	{
		prn.gprint( "\x1B\x17" ); // Elite
		prn.gprint( title );
		prn.gprint( "\r\x1B\x13" ); // pica
		prn.gprint( "\x1B\x55" ); prn.gputc( 0x00 ); // bidirectional (light)
		prn.gprint( "\r\r" );
		const char *hexhdr = "    0 1 2 3 4 5 6 7 8 9 A B C D E F ";
		prn.gprint( "TANDY Character Set                     IBM-2 Character Set\r" );
		prn.gprint( hexhdr );
		prn.gprint( "    " );
		prn.gprint( hexhdr );
		prn.gprint( "\r" );

		string hexchrs( "0123456789ABCDEF" );
		for ( int i=2; i<16; ++i )
		{
			for ( int j=0; j<2; ++j )
			{
				prn.gprint( j ? "\x1B\x3A" : "\x1B\x3B" );
				prn.gputc( hexchrs[i] );
				prn.gprint( " : " );
				for ( int k=0; k<16; ++k )
				{
					prn.gputc( char(16*i+k) );
					prn.gputc( ' ' );
				}
				prn.gprint( j ? "\r" : "    " );
			}
		}

		prn.gprint( "\x1B\x55" ); prn.gputc( 0x01 ); // unidirectional (normal)
		prn.gprint( "\r\r" );

		const char *ruler =
			"----+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----P----+----9----+E---0"
			"----+----1----+----2----+----3--C-+----4"
			"\r\r";

		const char *styles =
			" \x1B\x1F""BOLD""\x1B\x20"" ""\x0F""UNDERLINED""\x0E\r";

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x13" ); // pica
		prn.gprint( "PICA" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x17" ); // Elite
		prn.gprint( "ELITE" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x14" ); // condensed
		prn.gprint( "CONDENSED" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\x1B\x0E" ); // elongated on

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x13" ); // pica
		prn.gprint( "PICA WIDE" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x17" ); // Elite
		prn.gprint( "ELITE WIDE" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\r\r" );
		prn.gprint( "\x1B\x14" ); // condensed
		prn.gprint( "CONDENSED WIDE" );
		prn.gprint( styles );
		prn.gprint( ruler );

		prn.gprint( "\x1B\x0F" ); // elongated off
	}
	else
	{
		while ( true )
		{
			int c = istr.get();
			if ( istr.eof() )
				break;
			prn.gputc( c );
		}
	}

	prn.eject();

	cerr << "Printed page(s): " << dec << prn.pages << endl;

	return prn.pages ? 0 : 9;

}
