/**
 * \file dummy_impl.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Jean-Marc Lasgouttes
 *
 * Full author contact details are available in file CREDITS.
 */

/**
 * This file contains dummy implementation of some methods that are
 * needed by classes used by tex2lyx. This allows to reduce the number
 * of classes we have to link against.
*/

// {[(

#include <config.h>

#include "../tex2lyx/tex2lyx.h"
#include "LaTeXColors.h"
#include "LaTeXFeatures.h"
#include "LyXRC.h"
#include "output_xhtml.h"
#include "xml.h"

#include "support/lstrings.h"
#include "support/Lexer.h"
#include "support/Messages.h"

#include <iostream>

using namespace std;

namespace lyx {

using namespace support;

// Make linker happy

LaTeXColors & theLaTeXColors()
{
	LaTeXColors * lc = new LaTeXColors;
	return * lc;
}

//
// Dummy translation support (needed at many places)
//

bool LaTeXColors::isLaTeXColor(string const & /* name */)
{
	return false;
}

LaTeXColors::TexColorMap LaTeXColors::getLaTeXColors()
{
	// this is just an empty dummy,
	// the colors themselves are not needed
	static TexColorMap dummy_texcolormap;
	return dummy_texcolormap;
}

LaTeXColor LaTeXColors::getLaTeXColor(string const & /* name */)
{
	// the color itself is not needed
	return LaTeXColor();
}

bool LaTeXColor::read(Lexer & lex)
{
	if (!lex.next()) {
		lex.printError("No name given for LaTeX color: `$$Token'.");
		return false;
	}

	if (!readColor(lex)) {
		lex.printError("Error parsing Color: `$$Token'.");
		return false;
	}

	return true;
}


bool LaTeXColor::readColor(Lexer & lex)
{
	enum LaTeXColorTags {
		LC_CATEGORY = 1,
		LC_CMYK,
		LC_COLOR_MODEL,
		LC_END,
		LC_GUINAME,
		LC_HEXNAME,
		LC_LATEXNAME,
		LC_REQUIRES,
		LC_SVG_CLASH
	};

	// Keep these sorted alphabetically!
	LexerKeyword latexColorTags[] = {
		{ "category",             LC_CATEGORY },
		{ "cmyk",                 LC_CMYK },
		{ "colormodel",           LC_COLOR_MODEL },
		{ "endcolor",             LC_END },
		{ "guiname",              LC_GUINAME },
		{ "hexname",              LC_HEXNAME },
		{ "latexname",            LC_LATEXNAME },
		{ "requires",             LC_REQUIRES },
		{ "svgclash",             LC_SVG_CLASH },
	};

	bool error = false;
	bool finished = false;
	lex.pushTable(latexColorTags);
	// parse color section
	while (!finished && lex.isOK() && !error) {
		int le = lex.lex();
		// See comment in LyXRC.cpp.
		switch (le) {
		case Lexer::LEX_FEOF:
			continue;

		case Lexer::LEX_UNDEF: // parse error
			lex.printError("Unknown LaTeXColor tag `$$Token'");
			error = true;
			continue;

		default:
			break;
		}
		switch (static_cast<LaTeXColorTags>(le)) {
		case LC_END: // end of structure
			finished = true;
			break;
		case LC_CATEGORY:
		case LC_CMYK:
		case LC_COLOR_MODEL:
		case LC_LATEXNAME: {
			// check if this is an ASCII string
			lex.eatLine();
			string const val = lex.getString();
			if (!isAscii(val)){
				lex.printError("Value isn't ASCII: " + val);
				error = true;
				continue;
			}
			break;
		}
		case LC_HEXNAME: {
			// check if this is a Hex color
			lex.eatLine();
			docstring const val = trim(lex.getDocString(true));
			if (val.size() != 6 || !isHex(val)){
				lex.printError("Value isn't proper hex color: " + to_utf8(val));
				error = true;
				continue;
			}
			break;
		}
		case LC_SVG_CLASH: {
			// check if this is an ASCII string
			lex.eatLine();
			string const val = lowercase(lex.getString());
			if (val != "0" && val != "1" && val != "true" && val != "false") {
				lex.printError("Value isn't proper boolean: " + val);
				error = true;
				continue;
			}
			break;
		}
		case LC_GUINAME:
		case LC_REQUIRES: {
			// simply eat the value
			lex.eatLine();
			break;
		}
		}
	}
	if (!finished) {
		lex.printError("No EndColor tag found for LaTeXColor tag `$$Token'");
		return false;
	}
	lex.popTable();
	return finished && !error;
}

} // namespace lyx
