/**
 * \file SpecialChar.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Jürgen Spitzmüller
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "SpecialChar.h"

#include "support/Lexer.h"
#include "support/lstrings.h"

#ifdef ERROR
#undef ERROR
#endif

using namespace std;
using namespace lyx::support;

namespace lyx {

SpecialChars specialchars;

SpecialChar & SpecialChar::resolve(SpecialChar const & sc)
{
	if (sc.ignore_lang)
		// we ignore the language variant
		return const_cast<SpecialChar&>(sc);

	// inherit all non-default values
	if (lyx_output_default && !sc.lyx_output_default)
		lyx_output = sc.lyx_output;
	if (latex_output_default && !sc.latex_output_default)
		latex_output = sc.latex_output;
	if (latex_output_rtl_default && !sc.latex_output_rtl_default)
		latex_output_rtl = sc.latex_output_rtl;
	if (latex_output_utf8_default && !sc.latex_output_utf8_default)
		latex_output_utf8 = sc.latex_output_utf8;
	if (plaintext_output_default && !sc.plaintext_output_default)
		plaintext_output = sc.plaintext_output;
	if (xhtml_output_default && !sc.xhtml_output_default)
		xhtml_output = sc.xhtml_output;
	if (tooltip_default && !sc.tooltip_default)
		tooltip = sc.tooltip;
	if (menustring_default && !sc.menustring_default)
		menustring = sc.menustring;
	if (req_default && !sc.req_default)
		req = sc.req;
	if (type_default && !sc.type_default)
		type = sc.type;
	if (can_break_after_default && !sc.can_break_after_default)
		can_break_after = sc.can_break_after;
	if (is_letter_default && !sc.is_letter_default)
		is_letter = sc.is_letter;
	if (is_char_default && !sc.is_char_default)
		is_char = sc.is_char;
	if (need_protect_default && !sc.need_protect_default)
		need_protect = sc.need_protect;
	if (force_ltr_default && !sc.force_ltr_default)
		force_ltr = sc.force_ltr;
	if (font_default && !sc.font_default)
		font = sc.font;
	return *this;
}


SpecialChar SpecialChars::readSpecialChars(Lexer & lexrc, SpecialChar & sc) const
{
	enum {
		SC_LYX_OUTPUT,
		SC_LATEX_OUTPUT,
		SC_LATEX_OUTPUT_RTL,
		SC_LATEX_OUTPUT_UTF8,
		SC_PLAINTEXT_OUTPUT,
		SC_XHTML_OUTPUT,
		SC_TOOLTIP,
		SC_MENUSTRING,
		SC_REQUIRES,
		SC_FORCE_LTR,
		SC_IGNORE_LANG,
		SC_IS_CHAR,
		SC_IS_LETTER,
		SC_CAN_BREAK_AFTER,
		SC_FONT,
		SC_NEED_PROTECT,
		SC_TYPE,
		SC_END
	};

	LexerKeyword specialCharTags[] = {
		{"canbreakafter",   SC_CAN_BREAK_AFTER },
		{"end",             SC_END },
		{"font",            SC_FONT },
		{"forceltr",        SC_FORCE_LTR },
		{"ignorelanguage",  SC_IGNORE_LANG },
		{"ischar",          SC_IS_CHAR },
		{"isletter",        SC_IS_LETTER },
		{"latexoutput",     SC_LATEX_OUTPUT },
		{"latexoutputrtl",  SC_LATEX_OUTPUT_RTL },
		{"latexoutpututf8", SC_LATEX_OUTPUT_UTF8 },
		{"lyxoutput",       SC_LYX_OUTPUT },
		{"menustring",      SC_MENUSTRING },
		{"needprotect",     SC_NEED_PROTECT },
		{"plaintextoutput", SC_PLAINTEXT_OUTPUT },
		{"requires",        SC_REQUIRES },
		{"tooltip",         SC_TOOLTIP },
		{"type",            SC_TYPE },
		{"xhtmloutput",     SC_XHTML_OUTPUT }
	};

	lexrc.pushTable(specialCharTags);
	bool getout = false;
	while (!getout && lexrc.isOK()) {
		int le = lexrc.lex();
		switch (le) {
		case Lexer::LEX_UNDEF:
			lexrc.printError("Unknown SpecialChar tag `$$Token'");
			continue;
		default:
			break;
		}
		switch (le) {
		case SC_CAN_BREAK_AFTER:
			lexrc.next();
			sc.can_break_after = lexrc.getBool();
			sc.can_break_after_default = false;
			break;
		case SC_IGNORE_LANG:
			lexrc.next();
			sc.ignore_lang = lexrc.getBool();
			break;
		case SC_FORCE_LTR:
			lexrc.next();
			sc.force_ltr = lexrc.getBool();
			sc.force_ltr_default = false;
			break;
		case SC_IS_CHAR:
			lexrc.next();
			sc.is_char = lexrc.getBool();
			sc.is_char_default = false;
			break;
		case SC_IS_LETTER:
			lexrc.next();
			sc.is_letter = lexrc.getBool();
			sc.is_letter_default = false;
			break;
		case SC_NEED_PROTECT:
			lexrc.next();
			sc.need_protect = lexrc.getBool();
			sc.need_protect_default = false;
			break;
		case SC_MENUSTRING:
			lexrc.eatLine();
			sc.menustring = trim(lexrc.getString(), "\"");
			sc.menustring_default = false;
			break;
		case SC_LATEX_OUTPUT: {
			lexrc.next(true);
			docstring const res = rtrim(lexrc.getDocString());
			if (isHex(res))
				sc.latex_output = docstring(1, hexToInt(res));
			else
				sc.latex_output = rtrim(lexrc.getDocString());
			sc.latex_output_default = false;
			break;
		}
		case SC_LATEX_OUTPUT_RTL: {
			lexrc.next(true);
			docstring const res = rtrim(lexrc.getDocString());
			if (isHex(res))
				sc.latex_output_rtl = docstring(1, hexToInt(res));
			else
				sc.latex_output_rtl = rtrim(lexrc.getDocString());
			sc.latex_output_rtl_default = false;
			break;
		}
		case SC_LATEX_OUTPUT_UTF8: {
			lexrc.next(true);
			docstring const res = rtrim(lexrc.getDocString());
			if (isHex(res))
				sc.latex_output_utf8 = docstring(1, hexToInt(res));
			else
				sc.latex_output_utf8 = rtrim(lexrc.getDocString());
			sc.latex_output_utf8_default = false;
			break;
		}
		case SC_LYX_OUTPUT: {
			lexrc.next();
			docstring const res = rtrim(lexrc.getDocString());
			if (isHex(res))
				sc.lyx_output = docstring(1, hexToInt(res));
			else
				sc.lyx_output = rtrim(lexrc.getDocString());
			sc.lyx_output_default = false;
			break;
		}
		case SC_PLAINTEXT_OUTPUT: {
			lexrc.next();
			docstring const res = rtrim(lexrc.getDocString());
			if (isHex(res))
				sc.plaintext_output = docstring(1, hexToInt(res));
			else
				sc.plaintext_output = rtrim(lexrc.getDocString());
			sc.plaintext_output_default = false;
			break;
		}
		case SC_REQUIRES:
			lexrc.eatLine();
			sc.req = trim(lexrc.getString(), "\"");
			sc.req_default = false;
			break;
		case SC_TOOLTIP:
			lexrc.next();
			sc.tooltip = rtrim(lexrc.getDocString());
			sc.tooltip_default = false;
			break;
		case SC_TYPE:
			lexrc.next();
			sc.type = lowercase(lexrc.getString());
			sc.type_default = false;
			break;
		case SC_XHTML_OUTPUT:
			lexrc.next();
			sc.xhtml_output = rtrim(lexrc.getDocString());
			sc.xhtml_output_default = false;
			break;
		case SC_FONT:
			sc.font = lyxRead(lexrc, sc.font);
			sc.font_default = false;
			break;
		case SC_END:
			getout = true;
			break;
		}
	}
	return sc;
}

} // namespace lyx
