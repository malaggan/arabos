#ifndef KB_H
#define KB_H

#ifdef __cplusplus
extern "C" {
#endif

// since the known characters in there are ascii, then it is safe to use
// the range from 128 up for a special code for non-ascii keys
	enum KbSpecialKeys {VK_ESC=27, VK_CTRL = 128, VK_LSHIFT, VK_RSHIFT, VK_ALT,
	                    VK_CAPS_LOCK, VK_NUM_LOCK, VK_SCR_LOCK,
	                    VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6,
	                    VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
	                    VK_HOME, VK_END, VK_PAGE_DOWN, VK_PAGE_UP,
	                    VK_INSERT, VK_DELETE,
	                    VK_UP, VK_LEFT, VK_RIGHT, VK_DOWN,
	                    VK_UNDEFINED = 0 };

/* KBDUS means US Keyboard Layout. This is a scancode table
 *	used to layout a standard US keyboard. I have left some
 *	comments in to give you an idea of what key is what, even
 *	though I set it's array index to 0. You can change that to
 *	whatever you want using a macro, if you wish! */
	extern unsigned char KeyboardUs[128];

	void init_kb();

#ifdef __cplusplus
}
#endif

#endif //KB_H
