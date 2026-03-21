#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"

bool Hires::from_address(Memory::address offset, uint16_t &x, uint16_t &y) {
	if (offset >= 0x1f40) return false;	// screen hole

	uint8_t fine = (offset >> 10) & 0x07;	// bits 10-12
	uint8_t octad = (offset >> 4) & 0x07;	// bits 4-6
	uint8_t group = (offset >> 7) & 0x03;	// bits 7-8
	y = (group << 6) | (octad << 3) | fine;
	if (y >= 192) return false;

	x = 7 * ((offset % 128) % 40);
	return true;
}

Memory::address Hires::to_address(uint16_t y) {

	// Group (0, 64, or 128)
	uint16_t group = (y & 0xC0) << 4;       // y bits 6,7
	// Octad (0-7 within the group)
	uint16_t octad = (y & 0x38) << 1;       // y bits 3,4,5
	// Fine Y (the 8 scanlines of a character cell)
	uint16_t fine  = (y & 0x07) << 10;      // y bits 0,1,2
	return group + octad + fine;
}

void Hires::on_set(uint8_t b) {

	DBG_DSP("Hires::on_set");
	uint16_t x, y;
	if (from_address(_acc, x, y)) {
		uint16_t fg = _display.fg(), bg = _display.bg();
		for (uint8_t i = 0; i < 7; i++) {
			bool on = (b >> i) & 0x01;
			_display.drawPixel(x + i, y, on? fg: bg);
		}
	}
}

void Hires::redraw(uint8_t rowstart, uint8_t rowend) {

	DBG_DSP("Hires::redraw %d %d", rowstart, rowend);
	uint16_t x = 0;
	uint16_t y = 8*rowstart;
	uint16_t w = 280;
	uint16_t h = 8*(rowend - rowstart);
	_display.fillRect(x, y, w, h, _display.bg());
}
