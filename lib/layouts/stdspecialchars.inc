#
# Standard special characters of LyX
#

Format 114

Provides stdspecialchars 1


#
# Characters
#

SpecialChar endofsentence
	Type		specialchar
	MenuString	"End of Sentence|E"
	LyXOutput	0x002e
	LatexOutput	\\@.
	PlaintextOutput	0x002e
	XHTMLOutput	.
	Tooltip         "End of Sentence"
	IsChar		1
	Font
	  Color		special
	EndFont
End

SpecialChar ldots
	Type		specialchar
	MenuString	"Ellipsis|i"
	Tooltip         "Ellipsis"
	LyXOutput	0x2026
	LatexOutput	\\ldots
	PlaintextOutput	0x2026
	XHTMLOutput	&#x2026;
	IsChar		1
	Font
	  Color		special
	EndFont
End

SpecialChar menuseparator
	Type		specialchar
	MenuString	"Menu Separator|M"
	Tooltip         "Menu Separator"
	LyXOutput	0x25b7
	LatexOutput	\\lyxarrow
	LaTexOutputRTL	\\lyxarrow*
	PlaintextOutput	"->"
	XHTMLOutput	&#x21D2;
	Requires	lyxarrow
	CanBreakAfter   1
	IsChar		1
	Font
	  Color		special
	EndFont
End

SpecialChar breakableslash
	Type		specialchar
	MenuString	"Breakable Slash|a"
	Tooltip         "Breakable Slash"
	LyXOutput	0x002f
	LatexOutput	\\slash
	PlaintextOutput	0x002f
	XHTMLOutput	&#x2044;
	Tooltip         "Breakable Slash"
	CanBreakAfter   1
	IsChar		1
	Font
	  Color		special
	EndFont
End

SpecialChar nobreakdash
	Type		specialchar
	MenuString	"Non-Breaking Hyphen|y"
	Tooltip         "Non-Breaking Hyphen (SHY)"
	LyXOutput	0x2010
	LatexOutput	\\nobreakdash-
	NeedProtect	1
	PlaintextOutput	0x2011
	XHTMLOutput	&#x2011;
	Requires	amsmath
	IsLetter	1
	Font
	  Color		latex
	EndFont
	IsChar		1
End

#
# Formatting
#

SpecialChar allowbreak
	Type		formatting
	MenuString	"Optional Line Break|B"
	Tooltip         "Optional Line Break (ZWSP)"
	LyXOutput	0x002f
	LatexOutput	\\LyXZeroWidthSpace
	PlaintextOutput	0x200b
	XHTMLOutput	&#x200B;
	Requires        lyxzerowidthspace
	CanBreakAfter   1
	Font
	  Color		special
	EndFont
End

SpecialChar ligaturebreak
	Type		formatting
	MenuString	"Ligature Break|k"
	Tooltip         "Ligature Break (ZWNJ)"
	LyXOutput	0x007c
	LatexOutput	\\textcompwordmark
	LaTeXOutputUTF8 0x200c
	PlaintextOutput	0x200c
	XHTMLOutput	&#x200C;
	IsLetter	1
	Type		formatting
	Font
	  Color		special
	EndFont
End

SpecialChar softhyphen
	Type		formatting
	MenuString	"Exclusive Hyphenation Point|H"
	Tooltip         "Hyphenation point that overwrites predefined hyphenation possibilities"
	LyXOutput	0x2010
	LatexOutput	\\-
	IsLetter	1
	XHTMLOutput	&#xAD;
	CanBreakAfter   1
	Font
	  Color		special
	EndFont
End

#
# Logos
#

SpecialChar LaTeX
	Type		logo
	MenuString	"LaTeX Logo|a"
	LatexOutput	\\LaTeX
	NeedProtect	1
	PlaintextOutput	LaTeX
	XHTMLOutput	LaTeX
	IsLetter	1
	IsChar		1
	ForceLTR        1
End

SpecialChar LaTeX2e
	Type		logo
	MenuString	"LaTeX2e Logo|e"
	LatexOutput	\\LaTeXe
	NeedProtect	1
	PlaintextOutput	LaTeX2e
	XHTMLOutput	LaTeX2&#x03b5;
	IsLetter	1
	IsChar		1
	ForceLTR        1
End

SpecialChar LyX
	Type		logo
	MenuString	"LyX Logo|L"
	LatexOutput	\\LyX
	NeedProtect	1
	PlaintextOutput	LyX
	XHTMLOutput	LyX
	Requires	LyX
	IsLetter	1
	IsChar		1
	ForceLTR        1
End

SpecialChar TeX
	Type		logo
	MenuString	"TeX Logo|T"
	LatexOutput	\\TeX
	NeedProtect	1
	PlaintextOutput	TeX
	XHTMLOutput	TeX
	IsLetter	1
	IsChar		1
	ForceLTR        1
End

