#pragma once

#include "Bitmap.h"
#include "Writer.h"
#include "PackBits.h"
#include "DateTime.h"

#include <vector>

class TiffWriter
{
public:
	Bitmap &bmp;
	Writer w;

	TiffWriter( Bitmap &p_bitmap, ostream &p_ostr )
	: bmp( p_bitmap ), w( p_ostr ), nextifd( 0 )
	{
		init();
	}

	void init()
	{
		w.writeword( 0x4949 ); 	// 'II' Intel byte order
		w.writeword( 42 );		// Magic number

		nextifd = w.pos();
		w.writedword( 0 );
	}

	void print()
	{
		writeifd();

		// IFD
		int offset = w.pos();

		int ndirents = 13;
		offset += 2 + 12 * ndirents + 4 + 12/*filler*/ ;

		int imagewidth = bmp.width;
		int imageheight = bmp.height;

		// 1 = no compression; 0x8005 = packbits
		//int compression = 1;
		int compression = 0x8005;

		// Pack each row separately for Microsoft Office Document Imaging
		// (TIFF6.pdf Section 9: PackBits Compression, p.42 (!))
		int rowsperstrip = compression != 1 ? 1 : 16;

		int nstrips = ( imageheight + rowsperstrip - 1 ) / rowsperstrip;

		int stripoffsets = offset;
		offset += nstrips * 4;

		int stripbytecount = imagewidth * rowsperstrip / 8;

		int stripbytecounts = offset;
		offset += nstrips * 4;

		int xresolution = offset;
		offset += 8;

		int yresolution = offset;
		offset += 8;

		const char *psoftware = "DMP105toTiff";
		int software = offset;
		offset += align( strlen( psoftware ) + 1 );

		string pdatetime = DateTime().tiffDateTime();//"2024:08:25 13:58:59";
		int datetime = offset;
		offset += align( pdatetime.size() + 1 );

		int next = offset;
		vector< string > strips( nstrips );
		for ( int i=0; i<nstrips; ++i )
		{
			strips[i] = getstrip( i, stripbytecount, compression );
			next += align( strips[i].size() );
		}

		w.writeword( ndirents ); 									// # of dir entries
		w.writedirent( 0x00FE, 0x0004, 1, 0 );						// new subfile type
		w.writedirent( 0x0100, 0x0004, 1, imagewidth );				// image width
		w.writedirent( 0x0101, 0x0004, 1, imageheight );			// image length
		w.writedirent( 0x0103, 0x0004, 1, compression);				// compression
		w.writedirent( 0x0106, 0x0004, 1, 0);						// photometric interpretation
		w.writedirent( 0x0111, 0x0004, nstrips, stripoffsets );		// stripoffsets
		w.writedirent( 0x0116, 0x0004, 1, rowsperstrip );			// rowsperstrip
		w.writedirent( 0x0117, 0x0004, nstrips, stripbytecounts );	// stripbytescount
		w.writedirent( 0x011A, 0x0005, 1, xresolution );			// xresolution
		w.writedirent( 0x011B, 0x0005, 1, yresolution );			// yresolution
		w.writedirent( 0x0131, 0x0002, strlen(psoftware)+1, software );	// software
		w.writedirent( 0x0132, 0x0002, pdatetime.size()+1, datetime );	// datetime
		w.writedirent( 0x0128, 0x0003, 1, 2 );						// resolution unit
		nextifd = w.pos();
		w.writedword( 0 );											// next dirent

		w.fill( stripoffsets );

		// stripoffsets
		for ( int i=0; i<nstrips; ++i )
		{
			w.writedword( offset );
			offset += align( strips[i].size() );
		}

		// stripbytecounts
		for ( int i=0; i<nstrips; ++i )
		{
			w.writedword( strips[i].size() );
		}

		// xresolution
		w.writedword( bmp.ppih );
		w.writedword( 1 );

		// yresolution
		w.writedword( bmp.ppiv );
		w.writedword( 1 );

		// software
		w.writestr( psoftware );

		// datetime
		w.writestr( pdatetime.data() );

		for ( int i=0; i<nstrips; ++i )
		{
			w.align();
			for ( int j=0; j<strips[i].size(); ++j )
			{
				w.writebyte( strips[i][j] );
			}
		}
	}


private:

	string getstrip( int nstrip, int stripbytecount, int compression )
	{
		string ret;

		int p = nstrip * stripbytecount;

		switch( compression )
		{
		case 1: // no compression
		{
			for ( int i=0; i<stripbytecount; ++i )
				ret += p < bmp.size ? bmp.bitmap[p++] : 0;
			break;
		}
		case 0x8005:
		{

			int len =
				( p + stripbytecount < bmp.size )
				?	stripbytecount
				:	bmp.size - p;

			string str( (const char*)(bmp.bitmap+p), len );
			ret = PackBits().pack( (const byte *)str.data(), str.size(), 0, str.size() );
			break;
		}
		}
		return ret;
	}

	void writeifd()
	{
		size_t curpos = w.pos();
		w.seek( nextifd );
		w.writedword( curpos );
		w.seek( curpos );
	}

	int align( int offset )
	{
		return offset & 1 ? offset+1 : offset;
	}

	size_t nextifd;
};



