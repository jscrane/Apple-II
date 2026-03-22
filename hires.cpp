#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

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

void Hires::draw(uint8_t b, uint16_t x, uint16_t y) {

	uint16_t fg = _display.fg(), bg = _display.bg();
	for (uint8_t i = 0; i < CHAR_WIDTH; i++) {
		bool on = (b >> i) & 0x01;
		_display.drawPixel(x + i, y, on? fg: bg);
	}
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
