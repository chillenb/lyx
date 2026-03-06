/**
 * \file InsetTextbreak.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 * \author Jürgen Spitzmüller
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetTextbreak.h"

#include "Buffer.h"
#include "Cursor.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "MetricsInfo.h"
#include "ParIterator.h"
#include "Text.h"
#include "xml.h"
#include "texstream.h"
#include "TextMetrics.h"

#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"

#include "support/debug.h"
#include "support/docstring.h"
#include "support/docstream.h"
#include "support/gettext.h"
#include "support/Lexer.h"

using namespace std;

namespace lyx {

using support::Lexer;

InsetTextbreak::InsetTextbreak() : Inset(nullptr)
{}


InsetTextbreak::InsetTextbreak(InsetTextbreakParams const & params)
	: Inset(nullptr), params_(params)
{}


void InsetTextbreakParams::write(ostream & os) const
{
	switch (kind) {
	case InsetTextbreakParams::NEWPAGE:
		os << "newpage";
		break;
	case InsetTextbreakParams::PAGEBREAK:
		os <<  "pagebreak";
		break;
	case InsetTextbreakParams::CLEARPAGE:
		os <<  "clearpage";
		break;
	case InsetTextbreakParams::CLEARDOUBLEPAGE:
		os <<  "cleardoublepage";
		break;
	case InsetTextbreakParams::NOPAGEBREAK:
		os <<  "nopagebreak";
		break;
	case InsetTextbreakParams::CONTEXTUAL:
		os <<  "contextual";
		break;
	}
}


void InsetTextbreakParams::read(Lexer & lex)
{
	lex.setContext("InsetTextbreakParams::read");
	string token;
	lex >> token;

	if (token == "newpage")
		kind = InsetTextbreakParams::NEWPAGE;
	else if (token == "pagebreak")
		kind = InsetTextbreakParams::PAGEBREAK;
	else if (token == "clearpage")
		kind = InsetTextbreakParams::CLEARPAGE;
	else if (token == "cleardoublepage")
		kind = InsetTextbreakParams::CLEARDOUBLEPAGE;
	else if (token == "nopagebreak")
		kind = InsetTextbreakParams::NOPAGEBREAK;
	else if (token == "contextual")
		kind = InsetTextbreakParams::CONTEXTUAL;
	else
		lex.printError("Unknown kind");
}


void InsetTextbreak::write(ostream & os) const
{
	os << "Textbreak ";
	params_.write(os);
}


void InsetTextbreak::read(Lexer & lex)
{
	params_.read(lex);
	lex >> "\\end_inset";
}


void InsetTextbreak::metrics(MetricsInfo & mi, Dimension & dim) const
{
	if (params_.kind == InsetTextbreakParams::NOPAGEBREAK) {
		frontend::FontMetrics const & fm = theFontMetrics(mi.base.font);
		dim.asc = fm.maxAscent();
	        dim.des = fm.maxDescent();
	        dim.wid = 3 * fm.width('n');
		return;
	}

	dim.asc = defaultRowHeight();
	dim.des = defaultRowHeight();
	dim.wid = mi.base.textwidth;
}


void InsetTextbreak::draw(PainterInfo & pi, int x, int y) const
{
	if (params_.kind == InsetTextbreakParams::NOPAGEBREAK) {

	        FontInfo font;
	        font.setColor(ColorName());

	        frontend::FontMetrics const & fm = theFontMetrics(pi.base.font);
	        int const wid = 3 * fm.width('n');
	        int const asc = fm.maxAscent();

		int xp[3];
	        int yp[3];

		//left side arrow
		yp[0] = int(y - 0.875 * asc * 0.75);
		yp[1] = int(y - 0.500 * asc * 0.75);
		yp[2] = int(y - 0.125 * asc * 0.75);
		xp[0] = int(x + wid * 0.25);
		xp[1] = int(x + wid * 0.4); 
		xp[2] = int(x + wid * 0.25);
		pi.pain.lines(xp, yp, 3, ColorName());

		yp[0] = yp[1] = int(y - 0.500 * asc * 0.75);
		xp[0] = int(x + wid * 0.03);
		xp[1] = int(x + wid * 0.4); 
		pi.pain.lines(xp, yp, 2, ColorName());

		//right side arrow
		yp[0] = int(y - 0.875 * asc * 0.75);
		yp[1] = int(y - 0.500 * asc * 0.75);
		yp[2] = int(y - 0.125 * asc * 0.75);
		xp[0] = int(x + wid * 0.75);
		xp[1] = int(x + wid * 0.6); 
		xp[2] = int(x + wid * 0.75);
		pi.pain.lines(xp, yp, 3, ColorName());

		yp[0] = yp[1] = int(y - 0.500 * asc * 0.75);
		xp[0] = int(x + wid * 0.97);
		xp[1] = int(x + wid * 0.6); 
		pi.pain.lines(xp, yp, 2, ColorName());

		//mid-rule
		xp[0] = xp[1] = int(x + wid * 0.5);
		yp[0] = int(y - 0.875 * asc * 0.75);
		yp[1] = int(y - 0.125 * asc * 0.75);
		pi.pain.lines(xp, yp, 2, ColorName());
		return;
	}

	using frontend::Painter;

	FontInfo font;
	font.setColor(ColorName());
	font.decSize();

	Dimension const dim = dimension(*pi.base.bv);

	int w = 0;
	int a = 0;
	int d = 0;
	theFontMetrics(font).rectText(insetLabel(), w, a, d);

	int const text_start = int(x + (dim.wid - w) / 2);
	int const text_end = text_start + w;

	pi.pain.rectText(text_start, y + d, insetLabel(), font,
		Color_none, Color_none);

	pi.pain.line(x, y, text_start, y,
		   ColorName(), Painter::line_onoffdash);
	pi.pain.line(text_end, y, int(x + dim.wid), y,
		   ColorName(), Painter::line_onoffdash);
}


void InsetTextbreak::doDispatch(Cursor & cur, FuncRequest & cmd)
{
	switch (cmd.action()) {

	case LFUN_INSET_MODIFY: {
		InsetTextbreakParams params;
		cur.recordUndo();
		string2params(to_utf8(cmd.argument()), params);
		params_.kind = params.kind;
		break;
	}

	default:
		Inset::doDispatch(cur, cmd);
		break;
	}
}


bool InsetTextbreak::getStatus(Cursor & cur, FuncRequest const & cmd,
	FuncStatus & status) const
{
	switch (cmd.action()) {
	// we handle these
	case LFUN_INSET_MODIFY: {
		bool enabled = true;
		if (cmd.getArg(0) == "textbreak") {
			InsetTextbreakParams params;
			string2params(to_utf8(cmd.argument()), params);
			status.setOnOff(params_.kind == params.kind);
			enabled = params.kind != InsetTextbreakParams::CONTEXTUAL || !contextual_cmd_.empty();
		}
		status.setEnabled(enabled);
		return true;
	}
	default:
		return Inset::getStatus(cur, cmd, status);
	}
}


docstring InsetTextbreak::insetLabel() const
{
	switch (params_.kind) {
		case InsetTextbreakParams::NEWPAGE:
			return _("New Page");
		case InsetTextbreakParams::PAGEBREAK:
			return _("Page Break");
		case InsetTextbreakParams::CLEARPAGE:
			return _("Clear Page");
		case InsetTextbreakParams::CLEARDOUBLEPAGE:
			return _("Clear Double Page");
		case InsetTextbreakParams::NOPAGEBREAK:
			return _("No Page Break");
		case InsetTextbreakParams::CONTEXTUAL:
			return contextual_gui_.empty() ? _("Non-sensical Break") : _(contextual_gui_);
		default:
			return _("New Page");
	}
}


ColorCode InsetTextbreak::ColorName() const
{
	switch (params_.kind) {
		case InsetTextbreakParams::PAGEBREAK:
		case InsetTextbreakParams::NOPAGEBREAK:
			return Color_pagebreak;
		case InsetTextbreakParams::NEWPAGE:
		case InsetTextbreakParams::CLEARPAGE:
		case InsetTextbreakParams::CLEARDOUBLEPAGE:
		case InsetTextbreakParams::CONTEXTUAL:
			return Color_newpage;
	}
	// not really useful, but to avoids gcc complaints
	return Color_newpage;
}


void InsetTextbreak::latex(otexstream & os, OutputParams const & runparams) const
{
	if (runparams.inDeletedInset) {
		os << "\\mbox{}\\\\\\makebox[\\columnwidth]{\\dotfill\\ "
		   << insetLabel() << "\\ \\dotfill}";
	} else {
		switch (params_.kind) {
		case InsetTextbreakParams::NEWPAGE:
			os << "\\newpage" << termcmd;
			break;
		case InsetTextbreakParams::PAGEBREAK:
			if (runparams.moving_arg)
				os << "\\protect";
			os << "\\pagebreak" << termcmd;
			break;
		case InsetTextbreakParams::CLEARPAGE:
			os << "\\clearpage" << termcmd;
			break;
		case InsetTextbreakParams::CLEARDOUBLEPAGE:
			os << "\\cleardoublepage" << termcmd;
			break;
		case InsetTextbreakParams::NOPAGEBREAK:
			os << "\\nopagebreak" << termcmd;
			break;
		case InsetTextbreakParams::CONTEXTUAL:
			if (!contextual_cmd_.empty())
				os << "\\" << contextual_cmd_ << termcmd;
			break;
		default:
			os << "\\newpage" << termcmd;
			break;
		}
	}
}


int InsetTextbreak::plaintext(odocstringstream & os,
        OutputParams const &, size_t) const
{
	if (params_.kind ==  InsetTextbreakParams::NOPAGEBREAK)
		return 0;
	os << '\n';
	return PLAINTEXT_NEWLINE;
}


void InsetTextbreak::docbook(XMLStream & os, OutputParams const &) const
{
	if (params_.kind !=  InsetTextbreakParams::NOPAGEBREAK)
		os << xml::CR();
}


void InsetTextbreak::xhtml(XMLStream & xs, OutputParams const &) const
{
	if (params_.kind !=  InsetTextbreakParams::NOPAGEBREAK)
		xs << xml::CompTag("br");
}


void InsetTextbreak::updateBuffer(ParIterator const & it, UpdateType /* utype*/, bool const /*deleted*/)
{
	buffer().text().getContextualBreak(it.plist(), it.pit(), contextual_cmd_, contextual_gui_);
}


string InsetTextbreak::contextMenuName() const
{
	return "context-textbreak";
}


void InsetTextbreak::string2params(string const & in, InsetTextbreakParams & params)
{
	params = InsetTextbreakParams();
	if (in.empty())
		return;

	istringstream data(in);
	Lexer lex;
	lex.setStream(data);

	string name;
	lex >> name;
	if (!lex || name != "textbreak") {
		LYXERR0("Expected arg 2 to be \"textbreak\" in " << in);
		return;
	}

	params.read(lex);
}


string InsetTextbreak::params2string(InsetTextbreakParams const & params)
{
	ostringstream data;
	data << "textbreak" << ' ';
	params.write(data);
	return data.str();
}


} // namespace lyx
