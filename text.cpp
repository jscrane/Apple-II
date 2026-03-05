#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"
#include "text.h"
#include "charset.h"

void Text::draw(Memory::address a, uint8_t c) {

	uint8_t row, col;
	if (!map_address(a, row, col))
		return;

	uint8_t oc = _ram->get(a);
	uint16_t cc = CHAR_HEIGHT * c, cm = CHAR_HEIGHT * oc;
	uint16_t xc = col * CHAR_WIDTH, yc = row * CHAR_HEIGHT;
	uint16_t fg = _display.fg(), bg = _display.bg();
	bool invc = (c < 0x40), invoc = (oc < 0x40);
	if (invc) {
		uint16_t t = fg;
		fg = bg;
		bg = t;
	}
	for (uint8_t j = 0; j < CHAR_HEIGHT; j++) {
		uint8_t b = pgm_read_byte(&charset[cc + j]);
		uint8_t m = pgm_read_byte(&charset[cm + j]);
		if (b == m && invc == invoc)
			continue;

		if (invc != invoc)
			m = ~b;

		uint8_t d = (b ^ m);
		for (uint8_t i = 0, bit = 1; i < CHAR_WIDTH; i++, bit <<= 1)
			if (d & bit)
				_display.drawPixel(xc + CHAR_WIDTH - i, yc + j, (b & bit)? fg: bg);
	}
}
