// -*- C++ -*-
/**
 * \file SpecialChar.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef SPECIALCHAR_H
#define SPECIALCHAR_H

#include "FontInfo.h"

#include "support/docstring.h"

#ifdef ERROR
#undef ERROR
#endif

namespace lyx {

namespace support {
class Lexer;
}

class SpecialChar
{
public:
	docstring lyx_output;
	docstring latex_output;
	docstring latex_output_rtl;
	docstring latex_output_utf8;
	docstring plaintext_output;
	docstring xhtml_output;
	docstring tooltip;
	std::string menustring;
	std::string req;
	std::string type;
	bool can_break_after = false;
	bool is_letter = false;
	bool is_char = false;
	bool need_protect = false;
	bool force_ltr = false;
	FontInfo font;
};

class SpecialChars
{
public:
	///
	SpecialChar readSpecialChars(support::Lexer &, SpecialChar & sc) const;
};

/// Global singleton instance.
extern SpecialChars specialchars;


} // namespace lyx

#endif
