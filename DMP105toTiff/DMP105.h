#pragma once

#include "TiffWriter.h"

#define FONT_INCLUDE_DEFINITION

#include "font_tandy.h"
#include "font_ibm_2.h"
#include "font_robotron.h"


#include <iomanip>

/*
Width:
	PICA:		 960 = 15 * 64 = 450 * 64 / 30 = 28800 / 30
	ELITE:		1152 = 18 * 64 = 450 * 64 / 25 = 28800 / 25
	CONDENSED:	1600 = 25 * 64 = 450 * 64 / 18 = 28800 / 18

Char Pitch:
	PICA:	10CPI	= 30/3 CPI = 15/1.5
	ELITE:	12CPI	= 36/3 CPI = 18/1.5
	COND:	16.7CPI	= 50/3 CPI = 25/1.5

Dot Pitch:
	PICA:	10CPI	= 120 DPI = 8 * 15 DPI
	ELITE:	12CPI	= 144 DPI = 8 * 18 DPI
	COND:	16.7CPI	= 200 DPI = 8 * 25 DPI

Dot Size:
	PICA:	10CPI	= 1 / ( 8 * 15 ) " = 9 / 15 "/72
	ELITE:	12CPI	= 1 / ( 8 * 18 ) " = 9 / 18 "/72
	COND:	16.7CPI	= 1 / ( 8 * 25 ) " = 9 / 25 "/72

Text Mode:
	1/2 LF 	= 1/12"	=  6/72"
	1   LF 	= 1/6"  = 12/72"
	3/4 LF 	= 1/8"	=  9/72"

Graphics Mode:
	LF 				=  7/72"

*/


class DMP105
{
public:
	Bitmap bitmap;
	TiffWriter tiff;
	const FontInfo &font_tandy;
	const FontInfo &font_ibm_2;
	const FontInfo &font_robotron;
	const FontInfo *pFont;

	DMP105( ostream &ostr, int p_width, int p_height, int p_unit, int p_lines )
	: bitmap( p_width, p_height, p_unit,
		/*p_cols*/80, p_lines, /*p_dpch*/12, /*p_dpcv*/12, /*p_cpih*/10, /*p_cpiv*/6 )
	, font_tandy( FONT_TANDY )
	, font_ibm_2( FONT_IBM_2 )
	, font_robotron( FONT_ROBOTRON )
	, pFont( &font_tandy )
	, tiff( bitmap, ostr )
	, dirty_( false )
	{
		xsize = bitmap.xmaxsize;
		xsizeden = 15;
		xsizenum = xsize * xsizeden;

		ysize = bitmap.ymaxsize;
		ysizeden = 1;
		ysizenum = ysize * ysizeden;

		xpitch = 12; // dots per char
		ypitch = 12; // dots per line 12/72"

		xposmax = 80 * xpitch;
		yposmax = 72 * ypitch;

		xmargin = ( bitmap.width  - 80 * xpitch * bitmap.xmaxsize ) / 2;
		ymargin = ( bitmap.height - 72 * ypitch * bitmap.ymaxsize ) / 2;

		xposmin = 0;
		yposmin = 0;


		toplines = 0;
		skiplines = 0;

		cr_crlf = true;
		lf_crlf = false;
		graphics = false;
		elongated = false;
		weight = 2;
		unidir = true;
		underline = false;
		superscript = subscript = false;

		positioning = 0;
		repeat = 1;
		pages = 0;

		home();
	}

	~DMP105()
	{
		eject();
	}

	void home()
	{
		x = 0;
		xpos = xposmin;
		ypos = yposmin + toplines * ypitch;
	}

	void gputc( byte ch )
	{
		static byte esc = 0;

		byte ch7 = graphics ? ch : ch & 0x7F;

		if ( !esc && ( ch == 0x1B || ch == 0x1C ) ) // ESC / REPT
		{
			esc = ch;
			return;
		}
		else if ( esc == 0x1B )
		{
			switch ( ch )
			{
			case 0x0E:	// elongated on
				elongated = true;
				break;
			case 0x0F:	// elongated off
				elongated = false;
				break;
			case 0x13:	// standard
				xsize = 6;
				xpos = xpos * 15 / xsizeden;
				xsizeden = 15;
				xposmax = 80 * xpitch;
				break;
			case 0x14:	// condensed
				xsize = 3;
				xpos = xpos * 25 / xsizeden;
				xsizeden = 25;
				xposmax = 133 * xpitch;
				break;
			case 0x15:	// CR = CR
				cr_crlf = false;
				break;
			case 0x16:	// CR = CR + LF
				cr_crlf = true;
				break;
			case 0x17:	// elite
				xsize = 5;
				xpos = xpos * 18 / xsizeden;
				xsizeden = 18;
				xposmax = 96 * xpitch;
				break;
			case 0x1C:	// half LF
				ypitch = 6;
				break;
			case 0x1F:	// Bold on
				weight = 3;
				break;
			case 0x20:	// Bold off
				weight = unidir ? 2 : 1;
				break;
			case 0x36:	// full LF
				ypitch = 12;
				break;
			case 0x38:	// 3/4 LF
				ypitch = 9;
				break;
			case 0x3A:	// IBM char set
				pFont = &font_ibm_2;
				//pFont = &font_robotron;
				break;
			case 0x3B:	// Tandy char set
				pFont = &font_tandy;
				break;
			case 0x58:	// end super-/subscripting
				superscript = subscript = 0;
				break;
			case 0x10:	// positioning exec
			case 0x34:	// forms length set
			case 0x53:	// set super-/subscripting
			case 0x55:	// uni-/bidirectional set
			case 0x5A:	// forward feed n/72" exec
			case 0x5B:	// forward feed n/72" set
				esc = ch;
				return;
			}
		}
		else if ( esc == 0x1C )
		{
			esc = 0;
			repeat = ch;
			return;
		}
		else if ( esc == 0x10 ) // positioning MSB
		{
			esc = 0x11;
			positioning = ch << 8;
			return;
		}
		else if ( esc == 0x11 ) // positioning LSB
		{
			positioning |= ch;
			xpos = xposmin + positioning * 2;
		}
		else if ( esc == 0x53 ) // set super-/subscripting
		{
			if ( ch )
				subscript = true;
			else
				superscript = true;
		}
		else if ( esc == 0x55 ) // uni-/bidirectional set
		{
			unidir = ch;
			weight = unidir ? 2 : 1;
		}
		else if ( esc == 0x5A ) // Forward feed n/72" exec
		{
			ypos += ch;
		}
		else if ( esc == 0x5B ) // Forward feed n/72" set
		{
			ypitch = ch;
		}
		else if ( ch == 0x0A )
		{
			lf();
			if ( lf_crlf )
			{
				xpos = xposmin;
				x = 0;
			}
		}
		else if ( ch == 0x0C )	// Form Feed (extension)
		{
			eject();
		}
		else if ( ch == 0x0D )
		{
			xpos = xposmin;
			x = 0;
			if ( cr_crlf )
			{
				lf();
			}
		}
		else if ( ch == 0x12 )
		{
			graphics = true;
		}
		else if ( ch == 0x1E )
		{
			graphics = false;
		}
		else if ( graphics )
		{
			if ( ch > 127 )
			{
				for ( int r = 0; r<repeat; ++r )
				{
					byte b = ch;
					for ( int i=0; i<7; ++i )
					{
						if ( b & 0x01 )
							bitmap.set(
								xpos*xsizenum/xsizeden + xmargin,
								(ypos+i)*ysizenum/ysizeden + ymargin,
								xsize*weight,
								ysize );
						b >>= 1;
					}
					xpos += 2;
				}
				dirty_ = true;
			}
			else
			{
				cerr << "Ignored in Graphics mode: 0x" << hex << setw(2) << setfill('0') << short(ch) << endl;
			}
		}
		// Character-mode only control codes
		else if ( ch == 0x0E )
		{
			underline = false;
		}
		else if ( ch == 0x0F )
		{
			underline = true;
		}
		else if ( ch == 0x09 )	// Horizontal tab
		{
			while( x % 7 )
				gputc( ' ' );
		}
		else if ( ch == 0x08 )	// Backspace
		{
			--x;
			xpos -= xpitch * ( elongated ? 2 : 1 );
		}
		else if ( ch >= 0x20 )
		{
			int w = elongated ? 2 : 1;
			int h = superscript | subscript ? 1 : 2;
			int h0 = subscript ? pFont->height : 0;

			for ( int r = 0; r<repeat; ++r )
			{
				const byte *p = &pFont->data[ch*pFont->height*((pFont->width-1)/8+1)];
				for ( int i=0; i<pFont->height; ++i )
				{
					for ( int c=0; c<pFont->width; c+=8 )
					{
						byte b = *p++;
						for ( int j=c; j<c+8 && j<pFont->width; ++j )
						{
							if ( b & 0x80 )
							{
								for ( int k=0; k<w; ++k )
									bitmap.set(
										(xpos+w*(j+k))*xsizenum/xsizeden + xmargin,
										(ypos+i+h0)*ysizenum*h/2/ysizeden + ymargin,
										xsize*weight,
										ysize );
							}
							b <<= 1;
						}
					}
				}

				if ( underline )
				{
					for ( int c=0; c<pFont->width; ++c )
					{
						for ( int k=0; k<w; ++k )
							bitmap.set(
								(xpos+w*(c+k))*xsizenum/xsizeden + xmargin,
								(ypos+8)*ysizenum/ysizeden + ymargin,
								xsize*weight,
								ysize );
					}
				}

				xpos += xpitch * w;
				++x;

				if ( xpos >= xposmax )
				{
					xpos = xposmin;
					x = 0;
					lf();
				}
			}
			dirty_ = true;
		}
		else
		{
			cerr << "Ignored in Text mode: 0x" << hex << setw(2) << setfill('0') << short(ch) << endl;
		}

		esc = 0;
		repeat = 1;
	}

	void lf()
	{
		ypos += graphics ? 7 : ypitch;
		if ( ypos >= yposmax )
		{
			eject();
			ypos = yposmin; // LF should not advance by toplines
		}
		else if ( ypos >= yposmax - ypitch * ( skiplines - toplines ) )
		{
			eject(); // advance by toplines
		}
	}

	void gprint( const char *p )
	{
		for ( ; *p; ++p )
			gputc( *p );
	}

	void eject()
	{
		if ( dirty_ )
		{
			++pages;
			tiff.print();
			bitmap.clear();
			home();
		}
		dirty_ = false;
	}

	void setTopLines( int p_toplines )
	{
		toplines = p_toplines;
		while( ypos < toplines * ypitch )
			lf();
	}

	void setSkipLines( int p_skiplines )
	{
		skiplines = p_skiplines;
		//cerr << "SkipLines: " << skiplines << endl;
	}

	void setCrLf( bool p_crlf )
	{
		cr_crlf = p_crlf;
	}

	void setWeight( int p_weight )
	{
		weight = p_weight;
		unidir = weight != 1;
		//cerr << "Weight: " << weight << endl;
		//cerr << "Unidir: " << unidir << endl;
	}

	void setPitch( int p_pitch ) // p_pitch = Tandy ESC code to set pitch
	{
		if ( p_pitch == 0x13 || p_pitch == 0x14 || p_pitch == 0x17 )
		{
			gputc( 0x1B );
			gputc( p_pitch );
		}
		else if ( p_pitch ) // 0 ignored
		{
			cerr << "Bad pitch parameter: 0x" << hex << setfill('0') << setw(2) << p_pitch << endl;
		}
	}

	int pages;
private:
	int xsize;
	int xsizenum;
	int xsizeden;

	int ysize;
	int ysizenum;
	int ysizeden;

	int xpitch;
	int ypitch;

	int xmargin;
	int ymargin;
	int xposmin;
	int yposmin;
	int xposmax;
	int yposmax;

	int xpos;
	int ypos;

	int x;

	int toplines;
	int skiplines;

	int repeat;
	int positioning;

	bool cr_crlf;
	bool lf_crlf;
	bool graphics;
	bool elongated;
	int weight;
	bool unidir;
	bool underline;
	bool superscript;
	bool subscript;

	bool dirty_;
};

