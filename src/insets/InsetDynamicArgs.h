// -*- C++ -*-
/**
 * \file InsetDynamicArgs.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Jürgen Spitzmüller
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef INSETDYNAMICARGS_H
#define INSETDYNAMICARGS_H

#include "InsetCollapsible.h"


namespace lyx {

/** A derivate of collapsible with dynamic arguments

*/
class InsetDynamicArgs : public InsetCollapsible {
public:
	///
	InsetDynamicArgs(Buffer *);
	///
	bool getStatus(Cursor &, FuncRequest const &, FuncStatus &) const override;
	///
	void draw(PainterInfo & pi, int x, int y) const override;
	///
	void metrics(MetricsInfo &, Dimension &) const override;
	///
	Geometry geometry(BufferView const & bv) const;
	///
	bool canPaintChange(BufferView const & bv) const override;
	///
	bool editable() const override;
	/// can we go further down on mouse click?
	bool descendable(BufferView const & bv) const override;
	/// Returns true if coordinates are over the inset's button.
	/// Always returns false when the inset does not have a
	/// button.
	bool clickable(BufferView const & bv, int x, int y) const override;
	///
	void latex(otexstream &, OutputParams const &) const override;
	///
	std::string contextMenu(BufferView const & bv, int x, int y) const override;
	/// A simple command inset without any arguments
	bool isButtonOnly() const;

protected:
	///
	InsetDynamicArgs(InsetDynamicArgs const &);

private:
	///
	Inset * clone() const override { return new InsetDynamicArgs(*this); }
	///
	std::string name_;
};


} // namespace lyx

#endif
