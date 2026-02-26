#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>

#include "config.h"
#include "textscreen.h"
#include "charset.h"

#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

void TextScreen::begin() {

	_display.begin(BG_COLOUR, FG_COLOUR, ORIENT);
	_display.setScreen(CHAR_WIDTH * CHARS_PER_LINE, CHAR_HEIGHT * SCREEN_LINES);
	_display.clear();
}

void TextScreen::_set(Memory::address a, uint8_t c) {

	uint8_t oc = _mem[_acc];
	_mem[_acc] = c;

	if (_on && oc != c) {

		uint8_t x = (_acc % 128);
		if (x >= 120)	// "screen hole"?
			return;

		// display it
		uint8_t col = (x % CHARS_PER_LINE);
		uint8_t t = (x / CHARS_PER_LINE);	// "third"
		uint8_t l = (_acc / 128);		// "line group"
		uint8_t row = 8*t + l;

		DBG_EMU("setCursor: %d, %d", col, row);
		_display.setCursor(col * CHAR_WIDTH, row * CHAR_HEIGHT);

		// bit-7 is used for video effects
		// 0x00-0x3f: black text on white background
		// 0x40-0x7f: flashing
		// 0x80-0xff: normal: white text on black background

		char asc = (c & 0x3f);
		if (c < 0x20)
			asc = c | 0x40;

		DBG_EMU("print: %02x '%c'", asc, asc);
		_display.print(asc);
	}
}
