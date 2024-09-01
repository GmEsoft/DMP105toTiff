DMP105toTiff
============

Tandy DMP-105 Printer-to-TIFF Emulator


Description
-----------

This project is a pretty good emulator of the Tandy DMP-105 Dot Matrix Printer, that creates a TIFF file from the data sent to the printer. 
It also has some additional features found in its successor, the DMP-106.

The generated TIFF files consist in a set of pages encoded as a bichrome bitmap compressed using the Macintosh RLE algorithl a.k.a. PackBits.

These can be viewed and printed on Windows using the Microsoft Office Document Imaging tool, or other similar programs.

The project is written in standard C++98 using Visual Studio 2005, and should be easily portable to other versions or platforms.


Features
--------

- Almost complete support of the DMP-105 features, including the support of the graphics mode, the Pica, Elite and Condensed character sets,
bold, underline and elongated modes;
- The uni-/bidirectional printing modes are replaced with a print density (weight) setting, with normal and light dots;
- The Form Feed code, the horizontal tab and the backspace;
- The IBM Font Set 2 from the DMP-106 is partly implemented (but not the IBM mode);
- Super-/Subscript modes from the DMP-106;
- Some command-line switches to set the paper length, the top-of-page position, the skip lines, the density, the initial pitch, the CR=CF+LF flag;
- the possibility to take the input data from a file or from the standard input, allowing to pipe the printer output from a TRS-80 emulator to this program.


To do next
----------

 - Complete the design of the IBM Font Set 2;
 - Add the support of the  remaining DMP-106 control codes;
 - Add the IBM Mode support;
 - Perhaps add the Epson FX-80 emulation.


GPLv3 License
-------------

Created by Michel Bernard (michel_bernard@hotmail.com) -
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
