DMP105toTiff
============

Tandy DMP-105 Printer-to-TIFF Emulator


Description
-----------

This project is a pretty good emulator of the Tandy DMP-105 Dot Matrix Printer, that creates a TIFF file from the data sent to the printer. 
It also has some additional features found in its successor, the DMP-106.

The generated TIFF files consist in a set of pages encoded as a bilevel bitmap compressed using the Macintosh RLE algorithm a.k.a. PackBits.

These can be viewed and printed on Windows using the Microsoft Office Document Imaging tool, or other similar programs.

The project is written in standard C++98 using Visual Studio 2005, and should be easily portable to other versions or platforms. It has also been
successfully compiled with Visual Studio 2022.


Features
--------

- Almost complete support of the DMP-105 features, including the support of the graphics mode, the Pica, Elite and Condensed character sets,
Bold, Underlined and Elongated modes;
- The Uni-/Bidirectional Printing modes are replaced with a print density (weight) setting, with Normal and Light dots;
- The Form Feed code, the Horizontal Tab and the Backspace;
- The IBM Font Set 2 from the DMP-106 is partly implemented (but not the IBM mode);
- Super-/Subscript modes from the DMP-106;
- Some command-line switches to set the Paper Length, the Top-of-Page Position, the Skip Lines, the Density, the Initial Pitch, the CR=CF+LF Flag;
- the possibility to take the input data from a file or from the standard input, allowing to pipe the printer output from a TRS-80 emulator to this program.


Usage
-----

```
Usage:   DMP105toTiff [options] [-i:InputFile[.txt]] -o:OutputFile[.tif]
         -i:Inputfile[.txt]   input source file (default: stdin)
         -o:Outputfile[.tif]  output TIFF file
Options: -D   Self-test/Demo page and exit
         -T:n Top Lines
         -L   Lines per page [72]
         -P   Horizontal Pitch (P=Pica ,E=Elite, C=Condensed)
         -Wn  Dot Weight (1-3)
         -Q   Square pixels
         -C   CR=CR (default: CR=CR+LF)
         -Sn  Skip n lines at end of page
```

The tool can take `stdin` as input. Any program sending the printer output to `stdout` or `stderr` can be piped to this tool.

The `-Q` switch will allow to generate a TIFF file with square pixels. This will produce TIFF files about five times larger, but some applications
such as Windows 'Paint' will display them with the correct aspect ratio. Other applications such as 'Windows Photo Viewer' don't need that switch.


Control Codes
-------------

### Codes from the DMP-105

| Dec 			| Hex 			| Text Mode      		| Graphics Mode 				|
|--------------:|--------------:|:----------------------|:------------------------------|
|	1			|	01			| Ignored				| Ignored						|
|	2			|	02			| Ignored				| Ignored						|
|	10			|	0A			| Exec configured LF	| Exec LF 7/72"					|
|	138			|	8A			| id.					| Graphic data					|
|	13			|	0D			| CR (+LF if CR=CR+LF)	| CR (+LF 7/72" if CR=CR+LF)	|
|	141			|	8D			| id.					| Graphic data					|
|	14			|	0E			| End Underline			| Ignored						|
|	15			|	0F			| Start Underline		| Ignored						|
|	18			|	12			| Select Graphic Mode	| Ignored						|
|	27 14		|	1B 0E		| Start Elongation Mode	| Start Elongation Mode			|
|	27 15		|	1B 0F		| End Elongation Mode	| End Elongation Mode			|
|	27 16 n1 n2	|	1B 10 n1 n2	| Positioning			| Positioning					|
|	27 19		|	1B 13		| Sel Standard Char Set	| Ignored						|
|	27 20		|	1B 14		| Sel Condensed Char Set| Ignored						|
|	27 21		|	1B 15		| Set CR=CR				| Ignored						|
|	27 22		|	1B 16		| Set CR=CR+LF			| Ignored						|
|	27 23		|	1B 17		| Sel Elite Char Set	| Ignored						|
|	27 28		|	1B 1C		| Set Half-Forward LF	| Ignored						|
|	27 31		|	1B 1F		| Start Bold			| Ignored						|
|	27 32		|	1B 20		| End Bold				| Ignored						|
|	27 54		|	1B 36		| Set Full-Forward LF	| Ignored						|
|	27 56		|	1B 38		| Set 3/4-Forward LF	| Ignored						|
|	27 85 00 	|	1B 55 00	| Set Light Mode (*)	| Ignored						|
|	27 85 01	|	1B 55 01	| Set Normal Mode (*)	| Ignored						|
|	27 90 n 	|	1B 5A n		| Exec n/72" Forward LF	| Exec n/72" Forward LF			|
|	27 91 n 	|	1B 5B n		| Set n/72" Forward LF	| Ignored						|
|	28 n1 n2 	|	1C n1 n2	| Repeat Print Data		| Repeat Graphic Print Data		|
|	30			|	1E			| Ignored				| Ignored						|

(*) The Light/Normal modes replace the irrelevant Bi-/Unidirectioal Printing Modes.

### Additional Codes from the DMP-106

| Dec 			| Hex 			| Text Mode      		| Graphics Mode 				|
|--------------:|--------------:|:----------------------|:------------------------------|
|	27 52 n		|	1B 34 n		| Form Length Set		| Ignored						|
|	27 58		|	1B 3A		| Select IBM Char Set	| Select IBM Char set			|
|	27 59		|	1B 3B		| Select Tandy Char Set	| Select Tandy Char set			|
|	27 83 00	|	1B 53 00	| Select Superscript	| Ignored						|
|	27 83 01	|	1B 53 01	| Select Subscript		| Ignored						|
|	27 88		|	1B 58 01	| End Super-/Subscript	| Ignored						|

IBM Mode is currently not implemented.

### Additional Codes, originally not implemented (or undocumented?)

| Dec 			| Hex 			| Text Mode      		| Graphics Mode 				|
|--------------:|--------------:|:----------------------|:------------------------------|
|	08			|	08			| Backspace 1 char		| Ignored						|
|	09			|	09			| Horizontal Tab		| Ignored						|
|	12			|	0C			| Form Feed				| Ignored						|

The horizontal tab stops are set at positions 1, 9, 17, etc.


To do next
----------

 - Complete the design of the IBM Font Set 2;
 - Add the support of the  remaining DMP-106 control codes;
 - Add the IBM Mode support;
 - Perhaps add the Epson FX-80 emulation.


GPLv3 License
-------------

Created by Michel Bernard (michel_bernard@icloud.com) -
<http://www.github.com/GmEsoft/DMP105toTiff>

Copyright (c) 2024 Michel Bernard. All rights reserved.

This file is part of DMP105toTiff.

DMP105toTiff is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DMP105toTiff is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with DMP105toTiff.  If not, see <https://www.gnu.org/licenses/>.
