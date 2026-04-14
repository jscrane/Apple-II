#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "config.h"
#include "screen.h"

inline bool is_btm(uint16_t y) { return y >= SPLIT_LINE * CHAR_HEIGHT; }

bool Hires::from_address(Memory::address offset, uint16_t &x, uint16_t &y) {

	// Bits 10-12: Fine Y (scanline within an 8-line cell)
	uint16_t fine = (offset >> 10) & 0x07;
	// Bits 7-9: Octad (8-line "box" within a 64-line group)
	uint16_t octad = (offset >> 7) & 0x07;
	// Bits 0-6: The horizontal part, which also encodes the 64-line Group
	uint16_t low = offset & 0x7F;

	// The low 7 bits contain the byte column (0-39) and the group offset ($0, $28, $50)
	uint16_t xByte;
	uint16_t group;

	if (low < 40) {
		group = 0;
		xByte = low;
	} else if (low >= 48 && low < 80) {
		// This range contains Group 1 data AND Row 0's holes ($28-$2F)
		group = 64;
		xByte = low - 40;
	} else if (low >= 96 && low < 120) {
		// This range contains Group 2 data AND Row 64's holes ($50-$57)
		group = 128;
		xByte = low - 80;
	} else {
		// Final screen holes at the end of the 128-byte block ($78-$7F)
		return false;
	}

	y = group | (octad << 3) | fine;
	x = xByte * 7;

	return y < 192;
}

Memory::address Hires::to_address(uint16_t y) {

	// 1. Fine Y (bits 0,1,2): offset by 1024 bytes ($400 hex)
	uint16_t fine  = (y & 0x07) << 10;

	// 2. Octad (bits 3,4,5): offset by 128 bytes ($80 hex)
	uint16_t octad = (y & 0x38) << 4; // Shifted so bit 3 becomes value 128

	// 3. Group (bits 6,7): offset by 40 bytes ($28 hex)
	// Note: y/64 effectively isolates bits 6 and 7.
	uint16_t group = (y >> 6) * 0x28;

	return fine | octad | group;
}

#if defined(HIRES_COLOUR)
void Hires::redraw_row(uint16_t y) {

	Memory::address a = to_address(y);
	for (int xb = 0; xb < CHARS_PER_LINE; xb++, a++) {

		uint8_t b = _ram->get(a);
		bool bit7 = (b & 0x80);
		uint16_t xstart = xb * CHAR_WIDTH;

		// 9-bit sliding window: [NextBit] [7 Bits Current] [PrevBit]
		uint16_t window = (b & 0x7f) << 1;
		if (xb > 0)
			window |= (_ram->get(a-1) >> 6) & 1;
		if (xb < CHARS_PER_LINE-1)
			window |= ((_ram->get(a+1) & 1) << 8);

		for (int i = 0; i < CHAR_WIDTH; i++) {
			uint16_t pattern = (window >> i) & 7;	// [next][curr][prev]
			uint16_t x = xstart + i;
			uint16_t c = BLACK;
			if (pattern & 2) {		// curr is set
				if (pattern & 5)	// next or prev is also set
					c = WHITE;
				else {
					bool odd = (x & 1);
					if (bit7)
						c = odd? ORANGE: BLUE;
					else
						c = odd? GREEN: VIOLET;
				}
			}
			_display.drawPixel(x, y, c);
		}
	}
}

static uint32_t dirty_rows[6];
static int timer = -1;

void Hires::redraw_dirty() {
	for (int i = 0; i < 6; i++) {
		if (dirty_rows[i]) {
			for (int j = 0; j < 32; j++)
				if (dirty_rows[i] & (1 << j))
					redraw_row((i << 5) + j);
			_machine->yield();
			dirty_rows[i] = 0;
		}
	}
	timer = -1;
}
#endif

void Hires::draw(uint8_t b, uint16_t x, uint16_t y) {

#if defined(HIRES_MONO)
	uint16_t fg = _display.fg(), bg = _display.bg();
	for (uint8_t i = 0; i < CHAR_WIDTH; i++) {
		bool on = (b >> i) & 0x01;
		_display.drawPixel(x + i, y, on? fg: bg);
	}
#else
	dirty_rows[y >> 5] |= (1 << (y & 0x1f));
	if (timer < 0)
		timer = _machine->oneshot_timer(HIRES_REFRESH, [this]() { redraw_dirty(); });
#endif
}

// called when one or both of _top_active and _btm_active is set
// for hires, at least _top_active must be set, so only need to
// check _btm_active here.
void Hires::on_set(uint8_t b) {

	uint16_t x, y;
	if (from_address(_acc, x, y) && (!is_btm(y) || _btm_active))
		draw(b, x, y);
}

// only called from Screen::on_mode_change when required.
void Hires::redraw(uint8_t rowstart, uint8_t rowend) {

	DBG_DSP("Hires::redraw %d %d", rowstart, rowend);
	for (uint8_t row = rowstart; row < rowend; row++) {
		uint16_t y = CHAR_HEIGHT*row;
		Memory::address addr = to_address(y);
		for (uint8_t col = 0; col < CHARS_PER_LINE; col++)
			draw(_ram->get(addr + col), CHAR_WIDTH*col, y);
		_machine->yield();
	}
}
