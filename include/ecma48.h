#pragma once

/* ECMA values
   ref : http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html */

#define ECMA_PREFIX     "\033["
#define ECMA_SUFFIX	"m"
#define ECMA_SEPARATOR	";"

#define ECMA_SET_DEFAULT	"0"
#define ECMA_SET_BOLD		"1"
#define ECMA_SET_ITALICS	"3"
#define ECMA_SET_UNDERLINE	"4"
#define ECMA_SET_INVERTED_COLOR	"7"
#define ECMA_SET_STRIKETHROUGH	"9"

#define ECMA_UNSET_BOLD			"22"
#define ECMA_UNSET_ITALICS		"23"
#define ECMA_UNSET_UNDERLINE		"24"
#define ECMA_UNSET_INVERTED_COLOR	"27"
#define ECMA_UNSET_STRIKETHROUGH	"29"

#define ECMA_BACK_GND   "3"
#define ECMA_FORE_GND   "4"

#define ECMA_BLACK	"0"
#define ECMA_RED	"4" /*"1"*/
#define ECMA_GREEN	"2"
#define ECMA_YELLOW	"3"
#define ECMA_BLUE	"1" /*"4"*/
#define ECMA_MAGENTA	"5"
#define ECMA_CYAN	"6"
#define ECMA_WHITE	"7"
#define ECMA_DEFAULT	"9"

// some predefined attributes
// B for bold
#define NORMAL	ECMA_PREFIX ECMA_SET_DEFAULT ECMA_SUFFIX
#define WHITE	ECMA_PREFIX ECMA_BACK_GND ECMA_WHITE ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX
#define GREENB	ECMA_PREFIX ECMA_BACK_GND ECMA_GREEN ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX
#define REDB	ECMA_PREFIX ECMA_BACK_GND ECMA_RED   ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX
#define BLUEB	ECMA_PREFIX ECMA_BACK_GND ECMA_BLUE  ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX

unsigned char ECMA48(const char ** format);
