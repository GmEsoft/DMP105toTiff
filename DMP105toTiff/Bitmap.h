#pragma once

#include <cstring>

#include "TypeDefs.h"

class Bitmap
{
public:
	int xmaxsize;
	int xborder;
	int dpch;
	int dpih;
	int ppih;
	int width;

	int ymaxsize;
	int yborder;
	int dpcv;
	int dpiv;
	int ppiv;
	int height;

	byte *bitmap;
	size_t size;

	Bitmap( int p_width, int p_height, int p_unit, int p_cols, int p_lines,
		int p_dpch, int p_dpcv, int p_cpih, int p_cpiv )
	{
		// "dot" = printer dot  => dpi = printer dot per inch
		// "pix" = bitmap pixel => ppi = bitmap pixel per inch

											// for A4 paper size, PICA pitch
		xmaxsize = 6;						// 6 pixels per dot
		dpch = p_dpch; 						// 12 dots per char
		dpih = dpch * p_cpih;				// 12 * 10 = 120 dots per inch
		ppih = xmaxsize * dpih;				// 6 * 120 = 720 pixels per inch
		width = ppih * p_width / p_unit;	// 720 * 2100 / 254 = 5952 pixels wide

		ymaxsize = 2;						// 2 pixels per dot
		dpcv = p_dpcv;						// 12 dots per char
		dpiv = dpcv * p_cpiv;				// 12 * 6 = 72 dots per inch
		ppiv = ymaxsize * dpiv;				// 2 * 72 = 144 pixels per inch
		height = ppiv * p_height / p_unit;	// 144 * 3100 / 254 = 1757 hixels high

		size = height*(width>>3);
		bitmap = new byte[size];
	}

	~Bitmap()
	{
		delete[] bitmap;
	}

	void set( int x, int y )
	{
		if ( x >= 0 && x < width && y >= 0 && y < height )
		{
			int pos = ( x >> 3 ) + ( y * ( width >> 3 ) );
			if ( pos >= 0 && pos < size )
			{
				bitmap[pos] |= 0x80 >> ( x & 7 );
			}
		}
	}

	void set( int x, int y, int xsize, int ysize )
	{
		for ( int i=x; i<x+xsize; ++i )
			for ( int j=y; j<y+ysize; ++j )
				set( i, j );
	}

	void clear()
	{
		memset( bitmap, 0, size );
	}

};


