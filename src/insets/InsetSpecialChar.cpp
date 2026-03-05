/**
 * \file InsetSpecialChar.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Asger Alstrup Nielsen
 * \author Jean-Marc Lasgouttes
 * \author Jürgen Spitzmüller
 * \author Lars Gullik Bjønnes
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetSpecialChar.h"

#include "Buffer.h"
#include "BufferParams.h"
#include "Dimension.h"
#include "Encoding.h"
#include "Font.h"
#include "Language.h"
#include "LaTeXFeatures.h"
#include "MetricsInfo.h"
#include "Paragraph.h"
#include "ParIterator.h"
#include "xml.h"
#include "texstream.h"

#include "frontends/FontMetrics.h"
#include "frontends/NullPainter.h"
#include "frontends/Painter.h"

#include "support/debug.h"
#include "support/docstream.h"
#include "support/gettext.h"
#include "support/lstrings.h"
#include "support/textutils.h"
#include "support/Lexer.h"

using namespace std;
using namespace lyx::support;

namespace lyx {

using support::Lexer;


InsetSpecialChar::InsetSpecialChar(Buffer * buf, Language const * lang, string const & k)
	: Inset(buf), kind_(k), unknown_(false), lang_(const_cast<Language*>(lang))
{
	if (buf)
		update();
}


docstring InsetSpecialChar::toolTip(BufferView const &, int, int) const
{
	if (unknown_)
		return bformat(_("Unknown special character (%1$s)!"), from_utf8(kind_));

	return sc_.tooltip;
}


int InsetSpecialChar::rowFlags() const
{
	if (!unknown_ && sc_.can_break_after)
		return CanBreakAfter;

	return Inline;
}


namespace {

// helper function: draw text and update x.
void drawChar(PainterInfo & pi, int & x, int const y, char_type ch)
{
	FontInfo font = pi.base.font;
	font.setPaintColor(pi.textColor(font.realColor()));
	pi.pain.text(x, y, ch, font);
	x += theFontMetrics(font).width(ch);
}


void drawLogo(PainterInfo & pi, int & x, int const y, string const kind)
{
	FontInfo const & font = pi.base.font;
	int const em = theFontMetrics(font).em();

	if (kind == "LyX") {
		/** Reference macro:
		 *  \providecommand{\LyX}{L\kern-.1667em\lower.25em\hbox{Y}\kern-.125emX\\@};
		 */
		drawChar(pi, x, y, 'L');
		x -= em / 6;
		drawChar(pi, x, y + em / 4, 'Y');
		x -= em / 8;
		drawChar(pi, x, y, 'X');
		return;
	}
	if (kind == "TeX") {
		/** Reference macro:
		 *  \def\TeX{T\kern-.1667em\lower.5ex\hbox{E}\kern-.125emX\@}
		 */
		int const ex = theFontMetrics(font).xHeight();
		drawChar(pi, x, y, 'T');
		x -= em / 6;
		drawChar(pi, x, y + ex / 2, 'E');
		x -= em / 8;
		drawChar(pi, x, y, 'X');
		return;
	}
	if (kind == "LaTeX2e") {
		/** Reference macro:
		 *  \DeclareRobustCommand{\LaTeXe}{\mbox{\m@th
		 *    \if b\expandafter\@car\f@series\@nil\boldmath\fi
		 *    \LaTeX\kern.15em2$_{\textstyle\varepsilon}$}}
		 */
		drawLogo(pi, x, y, "LaTeX");
		x += 3 * em / 20;
		drawChar(pi, x, y, '2');
		// ε U+03B5 GREEK SMALL LETTER EPSILON
		drawChar(pi, x, y + em / 4, char_type(0x03b5));
		return;
	}
	if (kind == "LaTeX") {
		/** Reference macro:
		 * \DeclareRobustCommand{\LaTeX}{L\kern-.36em%
		 *        {\sbox\z@ T%
		 *         \vbox to\ht\z@{\hbox{\check@mathfonts
		 *                              \fontsize\sf@size\z@
		 *                              \math@fontsfalse\selectfont
		 *                              A}%
		 *                        \vss}%
		 *        }%
		 *        \kern-.15em%
		 *        \TeX}
		 */
		drawChar(pi, x, y, 'L');
		x -= 9 * em / 25;
		PainterInfo pi2 = pi;
		pi2.base.font.decSize().decSize();
		drawChar(pi2, x, y - em / 5, 'A');
		x -= 3 * em / 20;
		drawLogo(pi, x, y, "TeX");
		return;
	}
	LYXERR0("No information for drawing logo " << kind);
}

} // namespace


void InsetSpecialChar::metrics(MetricsInfo & mi, Dimension & dim) const
{
	frontend::FontMetrics const & fm = theFontMetrics(mi.base.font);
	dim.asc = fm.maxAscent();
	dim.des = 0;
	dim.wid = 0;

	docstring s;
	if (unknown_)
		s = from_ascii("??");
	else if (kind_ == "allowbreak") {
		dim.asc = fm.xHeight();
		dim.des = fm.descent('g');
		dim.wid = fm.em() / 8;
	} else if (kind_ == "menuseparator") {
		// ▹  U+25B9 WHITE RIGHT-POINTING SMALL TRIANGLE
		// There is a \thinspace on each side of the triangle
		dim.wid = 2 * fm.em() / 6 + fm.width(char_type(0x25B9));
	} else if (kind_ == "softhyphen") {
		dim.wid = fm.width(from_ascii("-"));
		if (dim.wid > 5)
			dim.wid -= 2; // to make it look shorter
	} else if (kind_ == "LyX" || kind_ == "TeX" || kind_ == "LaTeX" || kind_ == "LaTeX2e") {
		dim.asc = fm.maxAscent();
		dim.des = fm.maxDescent();
		frontend::NullPainter np;
		PainterInfo pi(mi.base.bv, np);
		pi.base.font = mi.base.font;
		// We rely on the fact that drawLogo updates x to compute
		// the width without code duplication.
		drawLogo(pi, dim.wid, 0, kind_);
	} else
		s = sc_.lyx_output;

	if (dim.wid == 0)
		dim.wid = fm.width(s);
}


void InsetSpecialChar::draw(PainterInfo & pi, int x, int y) const
{
	FontInfo font = pi.base.font;

	if (unknown_) {
		font.setColor(Color_error);
		pi.pain.text(x, y, from_ascii("??"), font);
		return;
	}

	if (kind_ == "allowbreak") {
		// A small vertical line
		int const asc = theFontMetrics(pi.base.font).xHeight();
		int const desc = theFontMetrics(pi.base.font).descent('g');
		int const x0 = x; // x + 1; // FIXME: incline,
		int const x1 = x; // x - 1; // similar to LibreOffice?
		int const y0 = y + desc;
		int const y1 = y - asc / 3;
		pi.pain.line(x0, y1, x1, y0, Color_special);
		return;
	}
	if (kind_ == "LyX" || kind_ == "TeX" || kind_ == "LaTeX" || kind_ == "LaTeX2e") {
		drawLogo(pi, x, y, kind_);
		return;
	}
	if (kind_ == "menuseparator") {
		frontend::FontMetrics const & fm = theFontMetrics(font);

		// There is a \thinspace on each side of the triangle
		x += fm.em() / 6;
		// ▹ U+25B9 WHITE RIGHT-POINTING SMALL TRIANGLE
		// ◃ U+25C3 WHITE LEFT-POINTING SMALL TRIANGLE
		char_type const c = pi.ltr_pos ? 0x25B9 : 0x25C3;
		font.setColor(Color_special);
		pi.pain.text(x, y, c, font);
		return;
	}

	font.setColor(sc_.font.color());
	pi.pain.text(x, y, sc_.lyx_output, font);
}


void InsetSpecialChar::write(ostream & os) const
{
	os << "\\SpecialChar " << kind_ << "\n";
}


void InsetSpecialChar::read(Lexer & lex)
{
	lex.next();
	kind_ = lex.getString();
	unknown_ = false;
}


void InsetSpecialChar::latex(otexstream & os, OutputParams const & rp) const
{
	if (unknown_)
		return;

	bool const rtl = rp.local_font && rp.local_font->isRightToLeft();
	bool const utf8 = rp.encoding->iconvName() == "UTF-8";
	bool force_ltr = false;
	string lswitch = "";
	string lswitche = "";
	if (rtl && !rp.use_polyglossia) {
		force_ltr = sc_.force_ltr;
		lswitch = "\\L{";
		lswitche = "}";
		if (getLocalOrDefaultLang(rp)->lang() == "arabic_arabi"
		    || getLocalOrDefaultLang(rp)->lang() == "farsi")
			lswitch = "\\textLR{";
	}

	if (sc_.need_protect && rp.moving_arg)
		os << "\\protect";
	if (force_ltr)
		os << lswitch;
	if (rtl && !sc_.latex_output_rtl.empty())
		os << sc_.latex_output_rtl;
	else if (utf8 && !sc_.latex_output_utf8.empty())
		os << sc_.latex_output_utf8;
	else
		os << sc_.latex_output;
	if (force_ltr)
		os << lswitche;
	else if (sc_.latex_output_utf8.empty()
		 && prefixIs(sc_.latex_output, from_ascii("\\"))
		 && isAlphaASCII(os.lastChar()))
		os << termcmd;
}


int InsetSpecialChar::plaintext(odocstringstream & os, OutputParams const &, size_t) const
{
	if (unknown_)
		return 0;

	docstring const res = sc_.plaintext_output;
	os << res;
	return res.size();
}


void InsetSpecialChar::docbook(XMLStream & xs, OutputParams const &) const
{
	if (unknown_)
		return;

	xs << XMLStream::ESCAPE_NONE << sc_.xhtml_output;
}


void InsetSpecialChar::xhtml(XMLStream & xs, OutputParams const &) const
{
	if (unknown_)
		return docstring();

	xs << XMLStream::ESCAPE_NONE << sc_.xhtml_output;
}


void InsetSpecialChar::update()
{
	bool const local = lang_ && lang_->isKnownSpecialChar(kind_);
	if (!local && !buffer().masterParams().documentClass().isKnownSpecialChar(kind_))
		unknown_ = true;
	else {
		sc_ = buffer().masterParams().documentClass().specialChars()[kind_];
		if (local) {
			SpecialChar lsc = lang_->specialChars()[kind_];
			sc_ = lsc.resolve(sc_);
		}
		unknown_ = false;
	}
}


void InsetSpecialChar::updateBuffer(ParIterator const & it, UpdateType /* utype*/, bool const /*deleted*/)
{
	BufferParams const & bp = buffer().params();
	lang_ = const_cast<Language *>(it.paragraph().getFontSettings(bp, it.pos()).language());
	update();
}


void InsetSpecialChar::toString(odocstream & os) const
{
	if (unknown_)
		return;

	if (kind_ == "allowbreak" || kind_ == "ligaturebreak")
		// Do not output ZERO WIDTH SPACE and ZERO WIDTH NON JOINER here
		// Spell checker would choke on it.
		return;

	odocstringstream ods;
	plaintext(ods, OutputParams(nullptr));
	os << ods.str();
}


void InsetSpecialChar::forOutliner(docstring & os, size_t const,
				   bool const) const
{
	if (unknown_)
		return;

	odocstringstream ods;
	plaintext(ods, OutputParams(nullptr));
	os += ods.str();
}


void InsetSpecialChar::validate(LaTeXFeatures & features) const
{
	if (unknown_)
		return;

	if (sc_.req.empty())
		return;
	vector<string> const reqs = getVectorFromString(sc_.req);
	for (auto const & s : reqs)
		features.require(s);
}


bool InsetSpecialChar::isChar() const
{
	if (unknown_)
		return false;

	return sc_.is_char;
}


bool InsetSpecialChar::isLetter() const
{
	if (unknown_)
		return false;

	return sc_.is_letter;
}


bool InsetSpecialChar::isLineSeparator() const
{
#if 0
	// this would be nice, but it does not work, since
	// Paragraph::stripLeadingSpaces nukes the characters which
	// have this property. I leave the code here, since it should
	// eventually be made to work. (JMarc 20020327)
	return kind_ == HYPHENATION || kind_ == ALLOWBREAK
	    || kind_ == MENU_SEPARATOR || kind_ == SLASH;
#else
	return false;
#endif
}


} // namespace lyx
