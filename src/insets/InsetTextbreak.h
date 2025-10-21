// -*- C++ -*-
/**
 * \file InsetTextbreak.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef INSET_TEXTBREAK_H
#define INSET_TEXTBREAK_H

#include "Inset.h"


namespace lyx {

class InsetTextbreakParams
{
public:
	/// The different kinds of breaks we support
	enum Kind {
		///
		NEWPAGE,
		///
		PAGEBREAK,
		///
		CLEARPAGE,
		///
		CLEARDOUBLEPAGE,
		///
		NOPAGEBREAK,
		///
		CONTEXTUAL
	};
	///
	InsetTextbreakParams() : kind(NEWPAGE) {}
	///
	void write(std::ostream & os) const;
	///
	void read(support::Lexer & lex);
	///
	Kind kind;
};


class InsetTextbreak : public Inset
{
public:
	///
	InsetTextbreak();
	///
	explicit InsetTextbreak(InsetTextbreakParams const & par);
	///
	static void string2params(std::string const &, InsetTextbreakParams &);
	///
	static std::string params2string(InsetTextbreakParams const &);
	/// Update the contextual information of this inset
	void updateBuffer(ParIterator const &, UpdateType, bool const deleted = false) override;

private:
	///
	InsetCode lyxCode() const override { return TEXTBREAK_CODE; }
	///
	void metrics(MetricsInfo &, Dimension &) const override;
	///
	void draw(PainterInfo & pi, int x, int y) const override;
	///
	void latex(otexstream &, OutputParams const &) const override;
	///
	int plaintext(odocstringstream & ods, OutputParams const & op,
	              size_t max_length = INT_MAX) const override;
	///
	void docbook(XMLStream &, OutputParams const &) const override;
	///
	docstring xhtml(XMLStream &, OutputParams const &) const override;
	///
	void read(support::Lexer & lex) override;
	///
	void write(std::ostream & os) const override;
	///
	int rowFlags() const override { return (params_.kind == InsetTextbreakParams::NOPAGEBREAK) ? Inline : Display; }
	///
	docstring insetLabel() const;
	///
	ColorCode ColorName() const;
	///
	std::string contextMenuName() const override;
	///
	Inset * clone() const override { return new InsetTextbreak(*this); }
	///
	void doDispatch(Cursor & cur, FuncRequest & cmd) override;
	///
	bool getStatus(Cursor & cur, FuncRequest const & cmd, FuncStatus &) const override;

	///
	InsetTextbreakParams params_;
	///
	std::string contextual_cmd_;
	///
	std::string contextual_gui_;
};

} // namespace lyx

#endif // INSET_TEXTBREAK_H
