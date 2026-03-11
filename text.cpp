#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"
#include "text.h"
#include "charset.h"

static inline bool is_flash(uint8_t b) { return b >= 0x40 && b < 0x80; }

static inline bool is_inverse(uint8_t b) { return b < 0x40; }

void Text::draw(uint8_t row, uint8_t col, uint8_t c, uint8_t) {

	uint16_t cc = CHAR_HEIGHT * (c & 0x3f);
	uint16_t xc = col * CHAR_WIDTH, yc = row * CHAR_HEIGHT;
	uint16_t fg = _display.fg(), bg = _display.bg();

	if (is_inverse(c))
		std::swap(fg, bg);

	for (uint8_t j = 0; j < CHAR_HEIGHT; j++) {
		uint8_t b = pgm_read_byte(&charset[cc + j]);
		for (uint8_t i = 0, bit = 1; i < CHAR_WIDTH; i++, bit <<= 1)
			_display.drawPixel(xc + CHAR_WIDTH - i, yc + j, (b & bit)? fg: bg);
	}
}

void Text::flash(uint8_t rowstart, uint8_t rowend, bool flash_is_inverse) {

	uint32_t bit = (1UL << rowstart);
	if (_flashrows < bit)
		return;

	for (uint8_t row = rowstart; row < rowend; row++, bit <<= 1)
		if (_flashrows & bit) {
			Memory::address rowaddr = to_address(row);
			for (uint8_t col = 0; col < CHARS_PER_LINE; col++) {
				uint8_t b = _ram->get(rowaddr + col);
				if (is_flash(b)) {
					uint8_t c = b & 0x3f;
					if (flash_is_inverse)
						c |= 0x80;
					draw(row, col, c, 0);
					yield();
				}
			}
		}
}

void Text::operator=(uint8_t c) {

	Screen::operator=(c);

	uint8_t row, col;
	if (from_address(_acc, row, col)) {
		uint8_t bit = 1 << row;
		if (is_flash(c))
			_flashrows |= bit;
		else if (_flashrows & bit) {
			Memory::address rowaddr = to_address(row);
			for (uint8_t col = 0; col < CHARS_PER_LINE; col++)
				if (is_flash(_ram->get(rowaddr + col)))
					return;
			_flashrows &= ~bit;
		}
	}
}
