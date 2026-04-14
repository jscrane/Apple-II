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
	if (offset >= 0x1f40) return false;	// screen hole

	uint8_t fine = (offset >> 10) & 0x07;	// bits 10-12
	uint8_t octad = (offset >> 7) & 0x07;	// bits 7-9
	uint8_t low = (offset & 0x7f);		// low 7 bits
	if (low >= 120) return false;		// screen hole

	uint8_t group = low / 40;		// 0, 1 or 2
	y = (group << 6) | (octad << 3) | fine;
	x = 7 * (low % 40);

	return y < SCREEN_LINES * CHAR_HEIGHT;
}

Memory::address Hires::to_address(uint16_t y) {

	// Group (0, 64, or 128)
	uint16_t group = (y / 64) * 40;
	// Octad (0-7 within the group)
	uint16_t octad = (y & 0x38) << 1;       // y bits 3,4,5
	// Fine Y (the 8 scanlines of a character cell)
	uint16_t fine  = (y & 0x07) << 10;      // y bits 0,1,2
	return group + octad + fine;
}

#if defined(HIRES_COLOUR)
#define HIRES_REFRESH	1000000
static uint32_t dirty_rows[6];
static int timer = -1;

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

void Hires::redraw_dirty() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 32; j++)
			if (dirty_rows[i] & (1 << j))
				redraw_row(j);
		dirty_rows[i] = 0;
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
	}
}
