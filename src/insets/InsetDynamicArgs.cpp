/**
 * \file InsetDynamicArgs.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Jürgen Spitzmüller
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetDynamicArgs.h"

#include "Buffer.h"
#include "BufferParams.h"
#include "BufferView.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "MetricsInfo.h"

#include "frontends/Painter.h"

using namespace std;

namespace lyx {


InsetDynamicArgs::InsetDynamicArgs(Buffer * buf)
	: InsetCollapsible(buf)
{}


InsetDynamicArgs::InsetDynamicArgs(InsetDynamicArgs const & in)
	: InsetCollapsible(in)
{}


bool InsetDynamicArgs::isButtonOnly() const
{
	InsetLayout const & il = getLayout();
	return (il.latextype() == InsetLaTeXType::SIMPLE_COMMAND
	        && (il.latexargs().empty() && il.postcommandargs().empty()));
}


void InsetDynamicArgs::draw(PainterInfo & pi, int x, int y) const
{
	if (!isButtonOnly()) {
		InsetCollapsible::draw(pi, x, y);
		return;
	}

	BufferView const & bv = *pi.base.bv;
	Changer dummy = pi.base.font.change(getFont(), true);
	// Draw button
	Dimension dimc = dimensionCollapsed(bv);

	FontInfo labelfont = getLabelfont();
	labelfont.setColor(labelColor());
	labelfont.realize(pi.base.font);
	pi.pain.buttonText(x, y, buttonLabel(bv), labelfont,
			   Color_commandbg, Color_commandframe, Inset::textOffset(pi.base.bv));
	// Draw the change tracking cue on the label, unless RowPainter already
	// takes care of it.
	if (canPaintChange(bv))
		pi.change.paintCue(pi, x, y, x + dimc.width(), labelfont);
}


void InsetDynamicArgs::metrics(MetricsInfo & mi, Dimension & dim) const
{
	if (!isButtonOnly()) {
		InsetCollapsible::metrics(mi, dim);
		return;
	}

	BufferView const & bv = *mi.base.bv;
	dim = dimensionCollapsed(bv);
}


bool InsetDynamicArgs::getStatus(Cursor & cur, FuncRequest const & cmd,
		FuncStatus & flag) const
{
	if (getLayout().latextype() == InsetLaTeXType::SIMPLE_COMMAND) {
		switch (cmd.action()) {
		// we only allow these for SimpleCommand
		case LFUN_ARGUMENT_INSERT:
		case LFUN_UNDO:
		case LFUN_REDO:
		case LFUN_CUT:
		case LFUN_WORD_DELETE_FORWARD:
		case LFUN_WORD_DELETE_BACKWARD:
		case LFUN_LINE_DELETE_FORWARD:
		case LFUN_WORD_FORWARD:
		case LFUN_WORD_BACKWARD:
		case LFUN_WORD_RIGHT:
		case LFUN_WORD_LEFT:
		case LFUN_CHAR_FORWARD:
		case LFUN_CHAR_FORWARD_SELECT:
		case LFUN_CHAR_BACKWARD:
		case LFUN_CHAR_BACKWARD_SELECT:
		case LFUN_CHAR_LEFT:
		case LFUN_CHAR_LEFT_SELECT:
		case LFUN_CHAR_RIGHT:
		case LFUN_CHAR_RIGHT_SELECT:
		case LFUN_UP:
		case LFUN_UP_SELECT:
		case LFUN_DOWN:
		case LFUN_PARAGRAPH_SELECT:
		case LFUN_LINE_BEGIN_SELECT:
		case LFUN_LINE_END_SELECT:
		case LFUN_WORD_FORWARD_SELECT:
		case LFUN_WORD_BACKWARD_SELECT:
		case LFUN_WORD_RIGHT_SELECT:
		case LFUN_WORD_LEFT_SELECT:
		case LFUN_WORD_SELECT:
		case LFUN_SECTION_SELECT:
		case LFUN_BUFFER_BEGIN:
		case LFUN_BUFFER_END:
		case LFUN_BUFFER_BEGIN_SELECT:
		case LFUN_BUFFER_END_SELECT:
		case LFUN_INSET_BEGIN:
		case LFUN_INSET_END:
		case LFUN_INSET_BEGIN_SELECT:
		case LFUN_INSET_END_SELECT:
		case LFUN_PARAGRAPH_UP:
		case LFUN_PARAGRAPH_DOWN:
		case LFUN_LINE_BEGIN:
		case LFUN_LINE_END:
		case LFUN_CHAR_DELETE_FORWARD:
		case LFUN_CHAR_DELETE_BACKWARD:
		case LFUN_SERVER_GET_XY:
		case LFUN_SERVER_SET_XY:
		case LFUN_SERVER_GET_LAYOUT:
		case LFUN_ESCAPE:
		case LFUN_SERVER_GET_STATISTICS:
			return InsetText::getStatus(cur, cmd, flag);

		case LFUN_INSET_DISSOLVE: {
			if (isButtonOnly()) {
				flag.setEnabled(false);
				return true;
			}
			return InsetText::getStatus(cur, cmd, flag);
		}

		default:
			flag.setEnabled(false);
			return true;
		}
	}

	return InsetCollapsible::getStatus(cur, cmd, flag);
}


InsetCollapsible::Geometry InsetDynamicArgs::geometry(BufferView const & bv) const
{
	if (isButtonOnly())
		return ButtonOnly;

	return InsetCollapsible::geometry(bv);
}


bool InsetDynamicArgs::editable() const
{
	if (isButtonOnly())
		return false;
	
	return InsetCollapsible::editable();
}


bool InsetDynamicArgs::descendable(BufferView const & bv) const
{
	if (isButtonOnly())
		return false;
	
	return InsetCollapsible::descendable(bv);
}


bool InsetDynamicArgs::clickable(BufferView const & bv, int x, int y) const
{
	if (isButtonOnly())
		return false;

	return InsetCollapsible::clickable(bv, x, y);
}


void InsetDynamicArgs::latex(otexstream & os, OutputParams const & runparams) const
{
	InsetLayout const & il = getLayout();
	if (il.latextype() != InsetLaTeXType::SIMPLE_COMMAND)
		// non-SimpleCommand collapsibles are
		// dealt with in InsetText
		return InsetCollapsible::latex(os, runparams);

	// SimpleCommand insets: They don't have a mandatory argument
	// from InsetText, but they might have InsetArguments
	if (il.forceOwnlines())
		os << breakln;

	if (!il.latexname().empty()) {
		// FIXME UNICODE
		// FIXME \protect should only be used for fragile
		//    commands, but we do not provide this information yet.
		if (runparams.moving_arg)
			os << "\\protect";
		os << '\\' << from_utf8(il.latexname());
		if (!il.latexargs().empty())
			getArgs(os, runparams);
		if (!il.latexparam().empty())
			os << from_utf8(il.latexparam());
	} else {
		if (!il.latexargs().empty())
			getArgs(os, runparams);
		if (!il.latexparam().empty())
			os << from_utf8(il.latexparam());
	}

	if (!il.latexname().empty()) {
		if (!il.postcommandargs().empty())
			getArgs(os, runparams, true);
	}

	if (il.forceOwnlines())
		os << breakln;
	else if (os.lastChar() != '}' && os.lastChar() != ']')
		// properly terminate command
		os << termcmd;
}


string InsetDynamicArgs::contextMenu(BufferView const & bv, int x, int y) const
{
	if (isButtonOnly())
		//no context menu (yet) for ButtonOnly insets
		return string();

	return InsetCollapsible::contextMenu(bv, x, y);
}

} // namespace lyx
