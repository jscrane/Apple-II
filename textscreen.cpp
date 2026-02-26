#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>

#include "config.h"
#include "textscreen.h"

void TextScreen::begin() {

	_display.begin(BG_COLOUR, FG_COLOUR, ORIENT, CHARS_PER_LINE, SCREEN_LINES);
}

void TextScreen::_set(Memory::address a, uint8_t c) {

	DBG_EMU("TextScreen::_set %04x %c", a, c);
	_mem[_acc] = c;
	if (_on) {
		// display it
	}
}
