# Standard RefPrefix definitions. 
# This defines the label prefixes for use with refstyle.sty.
#
# Author: Richard Kimberly Heck <rikiheck@lyx.org>
#
# Note that this file is included in stdsections.inc, which is included
# in stdclass.inc, so any file that includes either of these will get
# the standard RefPrefix definitions.

Format 116

ModifyStyle Part
	RefPrefix part
	CrossrefNeedDef       prettyref
End

ModifyStyle Chapter
	RefPrefix chap
	CrossrefNeedDef       prettyref
End

ModifyStyle Section
	RefPrefix sec
	CrossrefNeedDef       prettyref
End

ModifyStyle Subsection
	RefPrefix subsec
	CrossrefNeedDef       prettyref
End

ModifyStyle Subsubsection
	RefPrefix subsec
	CrossrefNeedDef       prettyref
End

ModifyStyle Paragraph
	RefPrefix par
	CrossrefNeedDef       prettyref
End

ModifyStyle Subparagraph
	RefPrefix par
	CrossrefNeedDef       prettyref
End
