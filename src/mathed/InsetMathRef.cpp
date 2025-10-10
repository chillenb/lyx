/**
 * \file InsetMathRef.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathRef.h"

#include "BufferParams.h"
#include "BufferView.h"
#include "Buffer.h"
#include "Cursor.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "LaTeXFeatures.h"
#include "LyX.h"
#include "MathData.h"
#include "MathFactory.h"
#include "MathStream.h"
#include "MathSupport.h"
#include "ParIterator.h"
#include "PDFOptions.h"
#include "frontends/alert.h"
#include "xml.h"

#include "insets/InsetCommand.h"
#include "insets/InsetRef.h"

#include "support/debug.h"
#include "support/gettext.h"
#include "support/lstrings.h"

#include <ostream>

using namespace std;
using namespace lyx::support;

namespace lyx {

InsetMathRef::InsetMathRef(Buffer * buf)
	: InsetMathCommand(buf, from_ascii("ref"), false, 3)
{}


InsetMathRef::InsetMathRef(Buffer * buf, docstring const & data)
	: InsetMathCommand(buf, data, false, 3)
{}


Inset * InsetMathRef::clone() const
{
	return new InsetMathRef(*this);
}


void InsetMathRef::infoize(odocstream & os) const
{
	os << "Ref: " << cell(0);
}


void InsetMathRef::doDispatch(Cursor & cur, FuncRequest & cmd)
{
	// Ctrl + click: go to label
	if (cmd.action() == LFUN_MOUSE_RELEASE && cmd.modifier() == ControlModifier) {
		LYXERR0("trying to goto ref '" << to_utf8(asString(cell(0))) << "'");
		//FIXME: use DispatchResult argument
		lyx::dispatch(FuncRequest(LFUN_LABEL_GOTO, asString(cell(0))));
		return;
	}

	switch (cmd.action()) {
	case LFUN_INSET_MODIFY: {
		string const arg0 = cmd.getArg(0);
		string const arg1 = cmd.getArg(1);
		if (arg0 == "ref") {
			if (arg1 == "changetarget") {
				string const oldtarget = cmd.getArg(2);
				string const newtarget = cmd.getArg(3);
				if (!oldtarget.empty() && !newtarget.empty()
				    && asString(cell(0)) == from_utf8(oldtarget))
					changeTarget(from_utf8(newtarget));
				cur.forceBufferUpdate();
				break;
			}
			MathData md(buffer_);
			if (createInsetMath_fromDialogStr(cmd.argument(), md)) {
				cur.recordUndo();
				Buffer & buf = buffer();
				*this = *md[0].nucleus()->asRefInset();
				setBuffer(buf);
				break;
			}
		} else if (arg0 == "changetype") {
			docstring const data = from_ascii(createDialogStr(arg1));
			MathData md(buffer_);
			if (createInsetMath_fromDialogStr(data, md)) {
				cur.recordUndo();
				Buffer & buf = buffer();
				*this = *md[0].nucleus()->asRefInset();
				setBuffer(buf);
				break;
			}
		}
		cur.undispatched();
		break;
	}

	case LFUN_INSET_DIALOG_UPDATE: {
		string const data = createDialogStr();
		cur.bv().updateDialog("ref", data);
		break;
	}

	case LFUN_INSET_SETTINGS: {
		string const data = createDialogStr();
		cur.bv().showDialog("ref", data, this);
		cur.dispatched();
		break;
	}

	case LFUN_MOUSE_RELEASE:
		if (cur.selection()) {
			cur.undispatched();
			break;
		}
		if (cmd.button() == mouse_button::button1) {
			string const data = createDialogStr();
			cur.bv().showDialog("ref", data, this);
			break;
		}
		cur.undispatched();
		break;

	case LFUN_MOUSE_PRESS: {
		bool do_selection = cmd.button() == mouse_button::button1
			&& cmd.modifier() == ShiftModifier;
		// For some reason the cursor points inside the first cell, which is not
		// active.
		cur.leaveInset(*this);
		cur.bv().mouseSetCursor(cur, do_selection);
		break;
	}

	case LFUN_MOUSE_DOUBLE:
	case LFUN_MOUSE_TRIPLE:
		// eat other mouse commands
		break;

	default:
		InsetMathCommand::doDispatch(cur, cmd);
		break;
	}
}


bool InsetMathRef::getStatus(Cursor & cur, FuncRequest const & cmd,
			 FuncStatus & status) const
{
	switch (cmd.action()) {
	// we handle these
	case LFUN_INSET_MODIFY:
		if (cmd.getArg(0) == "changetype")
			status.setOnOff(from_ascii(cmd.getArg(1)) == commandname());
		if (cmd.getArg(1) == "cpageref")
			status.setEnabled(buffer_
				&& (buffer().params().xref_package == "cleveref"
				    || buffer().params().xref_package == "zref"));
		else
			status.setEnabled(true);
		return true;
	case LFUN_INSET_DIALOG_UPDATE:
	case LFUN_INSET_SETTINGS:
	case LFUN_MOUSE_RELEASE:
	case LFUN_MOUSE_PRESS:
	case LFUN_MOUSE_DOUBLE:
	case LFUN_MOUSE_TRIPLE:
		status.setEnabled(true);
		return true;
	default:
		return InsetMathCommand::getStatus(cur, cmd, status);
	}
}


docstring const InsetMathRef::screenLabel() const
{
	docstring str;
	for (int i = 0; !types[i].latex_name.empty(); ++i) {
		if (commandname() == types[i].latex_name) {
			str = _(to_utf8(types[i].short_gui_name));
			break;
		}
	}
	str += asString(cell(0));

	return str;
}


void InsetMathRef::validate(LaTeXFeatures & features) const
{
	// This really should not happen here but does.
	if (!buffer_) {
		LYXERR0("Unassigned buffer_ in InsetMathRef::write!");
		LYXERR0("LaTeX output may be wrong!");
	}
	bool const use_refstyle =
		buffer_ && buffer().params().xref_package == "refstyle";

	if (commandname() == "vref" || commandname() == "vpageref") {
		if (buffer_ && buffer().masterParams().xref_package == "zref")
			features.require("zref-vario");
		else
			features.require("varioref");
	} else if (commandname() == "formatted") {
		if (use_refstyle)
			features.require("refstyle");
		else if (buffer_ && buffer().masterParams().xref_package == "cleveref")
			features.require("cleveref");
		else if (buffer_ && buffer().masterParams().xref_package == "zref")
			features.require("zref-clever");
		else
			features.require("prettyref");
	} else if (commandname() == "cpageref") {
		if (buffer_ && buffer().masterParams().xref_package == "cleveref")
			features.require("cleveref");
		else if (buffer_ && buffer().masterParams().xref_package == "zref")
			features.require("zref-clever");
	}
	// if eqref is used with refstyle, we do our own output
	else if (commandname() == "eqref" && use_refstyle)
		features.require("amsmath");
	else if (commandname() == "nameref")
		features.require("nameref");
}


void InsetMathRef::docbook(XMLStream & xs, OutputParams const &) const
{
	docstring attr = from_utf8("linkend=\"") + xml::cleanID(asString(cell(0))) + from_utf8("\"");
	xs << xml::CompTag("xref", to_utf8(attr));
}


void InsetMathRef::updateBuffer(ParIterator const & it, UpdateType /*utype*/, bool const /*deleted*/)
{
	if (!buffer_) {
		LYXERR0("InsetMathRef::updateBuffer: no buffer_!");
		return;
	}
	// register this inset into the buffer reference cache.
	buffer().addReference(getTarget(), this, it);
}


string const InsetMathRef::createDialogStr(string const & type) const
{
	InsetCommandParams icp(REF_CODE, (type.empty()
			?  to_ascii(commandname()) : type));
	icp["reference"] = asString(cell(0));
	if (!cell(1).empty())
		icp["options"] = asString(cell(1));
	if (hasFeature("plural"))
		icp["plural"] = from_ascii("true");
	if (hasFeature("caps"))
		icp["caps"] = from_ascii("true");
	if (hasFeature("noprefix"))
		icp["noprefix"] = from_ascii("true");
	if (hasFeature("nolink"))
		icp["nolink"] = from_ascii("true");
	if (hasFeature("range"))
		icp["tuple"] = from_ascii("range");
	return InsetCommand::params2string(icp);
}


docstring const InsetMathRef::getTarget() const
{
	return asString(cell(0));
}


bool InsetMathRef::hasFeature(string const & string) const
{
	vector<docstring> const features = getVectorFromString(asString(cell(2)));
	for (auto const & f : features) {
		if (from_ascii(string) == f)
			return true;
	}
	return false;
}


void InsetMathRef::changeTarget(docstring const & target)
{
	InsetCommandParams icp(REF_CODE, to_ascii(commandname()));
	icp["reference"] = target;
	if (!cell(1).empty())
		icp["options"] = asString(cell(1));
	if (hasFeature("plural"))
		icp["plural"] = from_ascii("true");
	if (hasFeature("caps"))
		icp["caps"] = from_ascii("true");
	if (hasFeature("noprefix"))
		icp["noprefix"] = from_ascii("true");
	if (hasFeature("nolink"))
		icp["nolink"] = from_ascii("true");
	if (hasFeature("range"))
		icp["tuple"] = from_ascii("range");
	MathData md(buffer_);
	Buffer & buf = buffer();
	if (createInsetMath_fromDialogStr(
	    from_utf8(InsetCommand::params2string(icp)), md)) {
		*this = *md[0].nucleus()->asRefInset();
		// FIXME audit setBuffer calls
		setBuffer(buf);
	}
}


bool InsetMathRef::useRange() const
{
	docstring const & cmd = commandname();
	vector<docstring> const refs = getVectorFromString(asString(cell(0)));
	if (refs.size() == 2 && (cmd == "vref" || cmd == "vpageref")
	    && buffer().masterParams().xref_package != "zref")
		return true;
	if (refs.size() != 2 || !hasFeature("range"))
		return false;
	return cmd == "vref" || cmd == "vpageref"
		|| (cmd == "formatted" && !prefixIs(buffer().masterParams().xref_package, "prettyref"))
		|| (cmd == "cpageref");
}


void InsetMathRef::writeMath(TeXMathStream & os) const
{
	docstring const & cmd = commandname();
	// This should not happen, but of course it does
	if (!buffer_) {
		LYXERR0("Unassigned buffer_ in InsetMathRef::write!");
		LYXERR0("LaTeX output may be wrong!");
	}
	if (!os.latex()) {
		// we are writing to the LyX file
		ModeSpecifier specifier(os, currentMode(), lockedMode(), asciiOnly());
		MathEnsurer ensurer(os, false);
		os << '\\' << cmd;
		if (!cell(1).empty())
			os << '[' << cell(1) << ']';
		os << '{' << cell(0) << '}';
		if (!cell(2).empty())
			os << '[' << cell(2) << ']';
		return;
	}
	bool const use_prettyref =
		prefixIs(buffer().masterParams().xref_package, "prettyref");
	bool const use_refstyle =
		buffer_ && buffer().params().xref_package == "refstyle";
	bool const use_cleveref = buffer().masterParams().xref_package == "cleveref";
	bool const use_zref = buffer().masterParams().xref_package == "zref";
	vector<docstring> labels = getVectorFromString(asString(cell(0)));
	int const nlabels = labels.size();
	bool const use_nolink = buffer().masterParams().pdfoptions().use_hyperref && hasFeature("nolink");
	// we need to translate 'formatted' to the appropriate commands
	// (depending on xref package) and just output the label with labelonly
	// most of this is borrowed from InsetRef and should be kept in 
	// sync with that.
	ModeSpecifier specifier(os, currentMode(), lockedMode(), asciiOnly());
	MathEnsurer ensurer(os, false);
	if (cmd == "vref" || cmd == "vpageref") {
		os << "\\";
		if (use_zref)
			os << "z";
		os << cmd;
		if (useRange())
			os << "range";
		if (use_nolink)
			os << "*";
		docstring opts = asString(cell(1));
		if (use_zref && hasFeature("caps")) {
			if (!opts.empty())
				opts +=", ";
			opts += "S";
		}
		if (use_zref && !opts.empty())
			os << "[" << opts << "]";
		bool first = true;
		os << "{";
		for (auto const & l : labels) {
			if (!first) {
				if (useRange())
					os << "}{";
				else
					os << ",";
			}
			os << l;
			first = false;
		}
		os << "}";
	} else if (cmd == "cpageref" && use_cleveref) {
		if (hasFeature("caps"))
			os << "\\Cpageref";
		else
			os << "\\cpageref";
		if (useRange())
			os << "range";
		bool first = true;
		os << "{";
		for (auto const & l : labels) {
			if (!first) {
				if (useRange())
					os << "}{";
				else
					os << ",";
			}
			os << l;
			first = false;
		}
		os << "}";
	} else if (cmd == "cpageref" && use_zref) {
		os << "\\zcpageref";
		if (use_nolink)
			os << "*";
		docstring opts = asString(cell(1));
		if (hasFeature("caps")) {
			if (!opts.empty())
				opts +=", ";
			opts += "S";
		}
		if (useRange()) {
			if (!opts.empty())
				opts +=", ";
			opts += "range";
		}
		if (!opts.empty())
			os << "[" << opts << "]";
		bool first = true;
		os << "{";
		for (auto const & l : labels) {
			if (!first)
				os << ",";
			os << l;
			first = false;
		}
		os << "}";
	} else if (cmd == "cpageref") {
		bool first = true;
		for (auto const & label : labels) {
			if (!first)
				os << ", ";
			os << "\\pageref";
			if (use_nolink)
				os << "*";
			os << '{' << label << '}';
			first = false;
		}
	} else if (nlabels > 1 && cmd == "ref" && use_cleveref) {
		os << "\\labelcref" << '{' << cell(0) << '}';
	} else if (nlabels > 1 && cmd == "pageref" && use_cleveref) {
		os << "\\labelcpageref" << '{' << cell(0) << '}';
	} else if (use_refstyle && cmd == "eqref") {
		// we advertise this as printing "(n)", so we'll do that, at least
		// for refstyle, since refstlye's own \eqref prints, by default,
		// "equation n". if one wants \eqref, one can get it by using a
		// formatted label in this case.
		os << '(' << from_ascii("\\ref");
		if (use_nolink)
			os << "*";
		os << "{" << cell(0) << from_ascii("})");
	} else if (cmd == "formatted") {
		odocstringstream ods;
		// get the label we are referencing
		for (auto const & d : cell(0))
			ods << d;
		docstring const ref = ods.str();

		vector<docstring> lbls;
		docstring prefix;
		docstring const fcmd =
			InsetRef::getFormattedCmd(ref, lbls, prefix,
						  buffer().params().xref_package,
						  hasFeature("caps"), useRange());
		os << fcmd;
		if (use_nolink && (use_cleveref || use_zref))
			os << "*";
		if (hasFeature("plural") && use_refstyle)
			os << "[s]";
		else if (use_zref) {
			docstring opts = asString(cell(1));
			if (hasFeature("caps")) {
				if (!opts.empty())
					opts +=", ";
				opts += "S";
			}
			if (useRange()) {
				if (!opts.empty())
					opts +=", ";
				opts += "range";
			}
			if (!opts.empty())
				os << "[" << opts << "]";
		}
		bool first = true;
		os << "{";
		vector<docstring>::const_iterator it = lbls.begin();
		vector<docstring>::const_iterator en = lbls.end();
		for (size_t i = 0; it != en; ++it, ++i) {
			if (!first) {
				if (use_prettyref) {
					os << "}";
					if (lbls.size() == 2)
						os << buffer().B_("[[reference 1]] and [[reference2]]");
					else if (i > 0 && i == lbls.size() - 1)
						os << buffer().B_("[[reference 1, ...]], and [[reference n]]");
					else
						os << buffer().B_("[[reference 1]], [[reference2, ...]]");
					os << "\\ref{";
				} else if (useRange() && !use_zref)
					os << "}{";
				else
					os << ",";
			}
			if (::contains(*it, ' ') && !useRange() && use_refstyle
			    && buffer().masterParams().isRefStyleSupported(prefix))
				// refstyle bug: labels with blanks need to be grouped for known commands
				// otherwise the blanks will be gobbled
				os << "{" << *it << "}";
			else {
				if (use_prettyref && !::contains(*it, ':'))
					// warn on invalid label
					frontend::Alert::warning(_("Invalid label!"),
								 bformat(_("The label `%1$s' does not have a prefix (e.g., `sec:'), "
									   "which is needed for formatted references with prettyref.\n"
									   "You will most likely run into a LaTeX error."),
									 *it), true);
				os << *it;
			}
			first = false;
		}
		os << "}";
	} else if (cmd == "labelonly") {
		if (!hasFeature("noprefix"))
			os << cell(0);
		else {
			docstring prefix;
			vector <docstring> slrefs;
			for (auto const & r : labels) {
				docstring suffix = split(r, prefix, ':');
				if (suffix.empty()) {
					LYXERR0("Label `" << r << "' contains no `:' separator.");
					slrefs.push_back(r);
				} else
					slrefs.push_back(suffix);
			}
			os << getStringFromVector(slrefs);
		}
	} else if (nlabels > 1 && cmd == "ref" && use_zref) {
		os << "\\zcref";
		if (use_nolink)
			os << "*";
		docstring opts = asString(cell(1));
		if (hasFeature("caps")) {
			if (!opts.empty())
				opts +=", ";
			opts += "noname";
		}
		if (use_zref && !opts.empty())
			os << "[" << opts << "]";
		os << '{' << cell(0) << '}';
	} else if (nlabels > 1 && cmd == "pageref" && use_zref) {
		os << "\\zcref";
		if (use_nolink)
			os << "*";
		docstring opts = asString(cell(1));
		if (!opts.empty())
			opts +=", ";
		opts += "noname, page";
		if (!opts.empty())
			os << "[" << opts << "]";
		os << '{' << cell(0) << '}';
	} else if (nlabels == 1) {
		os << "\\" << cmd;
		if (use_nolink)
			os << "*";
		os << "{" << cell(0) << "}";
	} else {
		bool first = true;
		vector<docstring>::const_iterator it = labels.begin();
		vector<docstring>::const_iterator en = labels.end();
		for (size_t i = 0; it != en; ++it, ++i) {
			if (!first) {
				if (labels.size() == 2)
					os << buffer().B_("[[reference 1]] and [[reference2]]");
				else if (i > 0 && i == labels.size() - 1)
					os << buffer().B_("[[reference 1, ...]], and [[reference n]]");
				else
					os << buffer().B_("[[reference 1]], [[reference2, ...]]");
			}
			os << "\\" << cmd;
			if (use_nolink)
				os << "*";
			os << '{' << *it << '}';
			first = false;
		}
	}
}


InsetMathRef::ref_type_info InsetMathRef::types[] = {
	{ from_ascii("ref"),       from_ascii(N_("Standard[[mathref]]")),   from_ascii(N_("Ref: "))},
	{ from_ascii("eqref"),     from_ascii(N_("Equation")),              from_ascii(N_("EqRef: "))},
	{ from_ascii("pageref"),   from_ascii(N_("Page Number")),           from_ascii(N_("Page: "))},
	{ from_ascii("cpageref"),  from_ascii(N_("Prefixed Page Number")),  from_ascii(N_("PrefPage: "))},
	{ from_ascii("vpageref"),  from_ascii(N_("Textual Page Number")),   from_ascii(N_("TextPage: "))},
	{ from_ascii("vref"),      from_ascii(N_("Standard+Textual Page")), from_ascii(N_("Ref+Text: "))},
	{ from_ascii("formatted"), from_ascii(N_("PrettyRef")),             from_ascii(N_("FormatRef: "))},
	{ from_ascii("nameref"),   from_ascii(N_("Reference to Name")),     from_ascii(N_("NameRef: "))},
	{ from_ascii("labelonly"), from_ascii(N_("Label Only")),             from_ascii(N_("Label Only: "))},
	{ from_ascii(""), from_ascii(""), from_ascii("") }
};


} // namespace lyx
