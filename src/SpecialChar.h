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
	bool ignore_lang = false;
	FontInfo font;
	// store whether we have default settings
	bool lyx_output_default = true;
	bool latex_output_default = true;
	bool latex_output_rtl_default = true;
	bool latex_output_utf8_default = true;
	bool plaintext_output_default = true;
	bool xhtml_output_default = true;
	bool tooltip_default = true;
	bool menustring_default = true;
	bool req_default = true;
	bool type_default = true;
	bool can_break_after_default = true;
	bool is_letter_default = true;
	bool is_char_default = true;
	bool need_protect_default = true;
	bool force_ltr_default = true;
	bool font_default = true;
	///
	SpecialChar & resolve(SpecialChar const &);
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
