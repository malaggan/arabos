#include <kb.h>
#include <console.h>
#include <port.h>
#include <idt.h>

int shift=0; // shift pressed ?
int ctrl=0;
int alt=0;
int numlock=0;
int capslock=0;
unsigned char* currKB=KeyboardUs;
unsigned char KeyboardUs[128] =
{
	0          , VK_ESC      , '1'      , '2'       , '3'      , '4'    , '5'   , '6'     , '7'         , '8'         ,
	'9'        , '0'         , '-'      , '='       , '\b'     , '\t'   , 'q'   , 'w'     , 'e'         , 'r'         ,
	't'        , 'y'         , 'u'      , 'i'       , 'o'      , 'p'    , '['   , ']'     , '\n'        , VK_CTRL     ,
	'a'        , 's'         , 'd'      , 'f'       , 'g'      , 'h'    , 'j'   , 'k'     , 'l'         , ';'         ,
	'\''       , '`'         , VK_LSHIFT, '\\'      , 'z'      , 'x'    , 'c'   , 'v'     , 'b'         , 'n'         ,
	'm'        , ','         , '.'      , '/'       , VK_RSHIFT, '*'    , VK_ALT, ' '     , VK_CAPS_LOCK, VK_F1       ,
	VK_F2      , VK_F3       , VK_F4    , VK_F5     , VK_F6    , VK_F7  , VK_F8 , VK_F9   , VK_F10      , VK_NUM_LOCK ,
	VK_SCR_LOCK, VK_HOME     , VK_UP    , VK_PAGE_UP, '-'      , VK_LEFT, 0     , VK_RIGHT, '+'         , VK_END      ,
	VK_DOWN    , VK_PAGE_DOWN, VK_INSERT, VK_DELETE , 0        , 0      , 0     , VK_F11  ,	VK_F12      , 0           ,
};

unsigned char shiftKeyboardUs[128] =
{
	0          , VK_ESC      , '!'      , '@'       , '#'      , '$'    , '%'   , '^'     , '&'         , '*'         ,
	'('        , ')'         , '_'      , '+'       , '\b'     , 0      , 'Q'   , 'W'     , 'E'         , 'R'         ,
	'T'        , 'Y'         , 'U'      , 'I'       , 'O'      , 'P'    , '{'   , '}'     , '\n'        , VK_CTRL     ,
	'A'        , 'S'         , 'D'      , 'F'       , 'G'      , 'H'    , 'J'   , 'K'     , 'L'         , ':'         ,
	'"'        , '~'         , VK_LSHIFT, '|'       , 'Z'      , 'X'    , 'C'   , 'V'     , 'B'         , 'N'         ,
	'M'        , '<'         , '>'      , '?'       , VK_RSHIFT, '*'    , VK_ALT, ' '     , VK_CAPS_LOCK, VK_F1       ,
	VK_F2      , VK_F3       , VK_F4    , VK_F5     , VK_F6    , VK_F7  , VK_F8 , VK_F9   , VK_F10      , VK_NUM_LOCK ,
	VK_SCR_LOCK, VK_HOME     , VK_UP    , VK_PAGE_UP, '-'      , VK_LEFT, 0     , VK_RIGHT, '+'         , VK_END      ,
	VK_DOWN    , VK_PAGE_DOWN, VK_INSERT, VK_DELETE , 0        , 0      , 0     , VK_F11  ,	VK_F12      , 0           ,
};

#include <asm.h>
static char* input = nullptr;
static int cnt_read = 0;
static int newline_read = 0; // boolean
static int freeze = 0; // input exceeded buffer, freeze additions
void readline(char* buf, int max)
{
	input = buf;
	while(1)
	{
		ASM("pause\n"
		    "hlt"); // hlt waits for interrupts

		if(newline_read)
			break;
		if(cnt_read == max)
			freeze=1;
	}
	*input++=0;

	// reset
	input = nullptr;
	cnt_read = 0;
	newline_read = 0;
	freeze = 0;
}

void readline_handler(unsigned char c)
{ // TODO handle backspaces
	if(nullptr == input)
		/* discard */;
	else
	{
		putchar(c);
		if('\n' == c)
			newline_read = 1;
		else if(!freeze)
			*input++ = c;
		cnt_read ++;
	}
}

// keyboard controller ref from: http://sardes.inrialpes.fr/~jphilipp/edu/cse07/doc/keyboard/Keyboard%20Reference.htm
int keyboard_handler(struct interrupt_frame * r/*unused*/)
{
	unsigned char status = inportb((unsigned short)0x64);
	if(!(status&0x1)) // has data ?
		return 0;
	unsigned char scancode = inportb((unsigned short)0x60);

	if (scancode & 0x80) // released
	{
		scancode &= 0x7F; // remove the release flag
		if(VK_LSHIFT == currKB[scancode] || VK_RSHIFT == currKB[scancode])
		{
			shift = 0;
			currKB = KeyboardUs;
		}
	}
	else
	{
		if(VK_LSHIFT == currKB[scancode] || VK_RSHIFT == currKB[scancode])
		{
			shift = 1;
			currKB = shiftKeyboardUs;
		}
		else
			readline_handler(currKB[scancode]);
	}
}

void init_kb()
{
	irq_install_custom_handler(1,keyboard_handler);
}
