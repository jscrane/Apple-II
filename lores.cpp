#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "config.h"
#include "screen.h"
#include "charset.h"

inline bool is_flash(uint8_t b) { return b >= 0x40 && b < 0x80; }

inline bool is_inverse(uint8_t b) { return b < 0x40; }

inline bool is_top(uint8_t row) { return row < SPLIT_LINE; }

bool Lores::from_address(Memory::address a, uint8_t &row, uint8_t &col) {

	uint8_t x = (a % 128);
	if (x >= 120)	// "screen hole"?
		return false;

	col = (x % CHARS_PER_LINE);
	uint8_t t = (x / CHARS_PER_LINE);	// "third"
	uint8_t l = (a / 128);			// "line group"
	row = 8*t + l;
	return true;
}

Memory::address Lores::to_address(uint8_t row) {

	return ((row & 7) << 7) | ((row & 24) * 5);
}

void Lores::on_page_change() {

	_flashrows = 0;

	uint32_t bit = 1;
	for (uint8_t row = 0; row < SCREEN_LINES; row++, bit <<= 1) {
		Memory::address rowaddr = to_address(row);
		for (uint8_t col = 0; col < CHARS_PER_LINE; col++)
			if (is_flash(_ram->get(rowaddr + col))) {
				_flashrows |= bit;
				break;
			}
	}
}

void Lores::on_set(uint8_t c) {

	uint8_t row, col;
	if (from_address(_acc, row, col)) {

		if (is_flash(c))
			_flashrows |= (1UL << row);

		bool top_row = is_top(row), btm_row = !top_row;

		if ((top_row && _top_active) || (btm_row && _btm_active)) {
			if ((top_row && _top_text) || (btm_row && _btm_text))
				draw_text(row, col, c);
			else
				draw_lores(row, col, c);
		}
	}
}

void Lores::redraw(uint8_t rowstart, uint8_t rowend, bool as_text) {

	DBG_DSP("Lores::redraw: %d %d", rowstart, rowend);

	if (is_top(rowstart)) _top_text = as_text;
	else _btm_text = as_text;

	for (uint8_t row = rowstart; row < rowend; row++) {
		Memory::address rowaddr = to_address(row);
		for (uint8_t col = 0; col < CHARS_PER_LINE; col++) {
			uint8_t c = _ram->get(rowaddr + col);
			if (as_text)
				draw_text(row, col, c);
			else
				draw_lores(row, col, c);
			yield();
		}
	}
}

void Lores::draw_lores(uint8_t row, uint8_t col, uint8_t c) {

	static uint16_t colours[] = {
		0x0000, 0xc197, 0x0014, 0xc1ff, 0x0029, 0x8410, 0x0019, 0x24ff,
		0x8200, 0xeb20, 0x8410, 0xf39d, 0x07e0, 0xffe0, 0x07ff, 0xffff
	};

	_display.fillRect(CHAR_WIDTH*col+1, CHAR_HEIGHT*row, CHAR_WIDTH, CHAR_HEIGHT / 2, colours[c & 0x0f]);
	_display.fillRect(CHAR_WIDTH*col+1, CHAR_HEIGHT*row+4, CHAR_WIDTH, CHAR_HEIGHT / 2, colours[c >> 4]);
}

void Lores::draw_text(uint8_t row, uint8_t col, uint8_t c) {

	if (is_top(row) && !_top_active) return;
	if (!is_top(row) && !_btm_active) return;

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

void Lores::flash_text(bool flash_is_inverse) {

	uint8_t rowstart = _top_text && _top_active? 0: SPLIT_LINE;
	uint8_t rowend = _btm_text? SCREEN_LINES: SPLIT_LINE;

	uint32_t bit = (1UL << rowstart);
	if (_flashrows < bit)
		return;

	for (uint8_t row = rowstart; row < rowend; row++, bit <<= 1)
		if (_flashrows & bit) {
			Memory::address rowaddr = to_address(row);
			bool found_flash = false;
			for (uint8_t col = 0; col < CHARS_PER_LINE; col++) {
				uint8_t b = _ram->get(rowaddr + col);
				if (is_flash(b)) {
					found_flash = true;
					uint8_t c = b & 0x3f;
					if (flash_is_inverse)
						c |= 0x80;
					draw_text(row, col, c);
					yield();
				}
			}
			if (!found_flash)
				_flashrows &= ~bit;
		}
}
