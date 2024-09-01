#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Globals

extern const char *DMP105_Font_Tandy[];
extern const char *DMP105_Font_IBM_2[];
extern const char *DMP105_Font_Robotron[];


const size_t nullptr = 0;

typedef unsigned char byte;

//////////////////////////////////////////////////////////////////////

void makeFont( const char * hppfilename, const char * fontfacesuffix, const char *font[] )
{
    std::cerr << "Preparing font " << hppfilename << std::endl;
    std::ofstream output;

    int xstep = 1;
    int ystep = 1;

	const int nsymbols = 256;

	byte raster[256][9*2] = {};
	
    int glyphcount = 0;

	int ascii = 0;

	for (int symbol = 0; symbol < nsymbols; symbol++)
    {
		const char **pchardata = font + symbol * 9;

        int y = 0;

		for (int line = 0; line < 9; line++)
        {
            const char *data = pchardata[line];
			size_t len = data ? strlen( data ) : 0;
			int i = 0, j = 0;
            for (unsigned col = 0; col < 12; col++)
            {
                int x = col;
                bool bit = col < len && data[col] != '.' ;
				raster[ascii][2*line+j] <<= 1;
                if (bit)
				{
					raster[ascii][2*line+j] |= 1;
				}
				++i;
				if ( i == 8 )
					++j, i = 0;
            }
			for ( ; i<8; ++i )
				raster[ascii][2*line+j] <<= 1;
            y += ystep;
        }

        glyphcount++;

		++ascii;
    }

	output.open( hppfilename );

	output << 
		"#pragma once\n"
		"\n"
		"#include \"fontinfo.h\"\n"
		"\n"
		"#ifdef FONT_INCLUDE_DEFINITION\n"
		"\n"
		"static const uint8_t FONT_" << fontfacesuffix << "_DATA[] = {\n";

	for ( int i=0; i<256; ++i )
	{
		for ( int j=0; j<9*2; ++j )
			output << hex << uppercase << "0x" << setw( 2 ) << setfill( '0' ) 
				<< short(raster[i][j]) << ", ";
		output << "// " << dec << setw( 3 ) << setfill( ' ' ) << i;
		if ( i>=32 )
			output << " '" << char(i) << "'";
		output << endl;
	}

	output << 
		"};\n"
		"\n"
		"extern const FontInfo FONT_" << fontfacesuffix <<" = {\n"
		"  /*.pointSize =*/ 0,\n"
		"  /*.width     =*/ 12,\n"
		"  /*.height    =*/ 9,\n"
		"  /*.ascent    =*/ 16,\n"
		"  /*.inleading =*/ 0,\n"
		"  /*.exleading =*/ 0,\n"
		"  /*.flags     =*/ 0,\n"
		"  /*.weight    =*/ 0,\n"
		"  /*.charset   =*/ 255,\n"
		"  /*.data      =*/ FONT_" << fontfacesuffix << "_DATA,\n"
		"  /*.chptr     =*/ 0,//nullptr,\n"
		"  /*.codepage  =*/ 437,\n"
		"};\n"
		"\n"
		"#else\n"
		"\n"
		"extern const FontInfo FONT_" << fontfacesuffix << ";\n"
		"\n"
		"#endif\n"
		"\n"
		"\n"
		;
	output.close();

    std::cerr << "Total glyph count: " << glyphcount << std::endl;
}

int main(int argc, char* argv[])
{
    std::cerr << "FontGen utility  by GmEsoft  " << __DATE__ << " " << __TIME__ << std::endl;

	makeFont( "font_tandy.h",		"TANDY",	DMP105_Font_Tandy		);
	makeFont( "font_ibm_2.h",		"IBM_2",	DMP105_Font_IBM_2		);
	makeFont( "font_robotron.h",	"ROBOTRON",	DMP105_Font_Robotron	);

}


//////////////////////////////////////////////////////////////////////
