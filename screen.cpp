#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "config.h"
#include "screen.h"

bool Screen::from_address(Memory::address a, uint8_t &row, uint8_t &col) {

	uint8_t x = (a % 128);
	if (x >= 120)	// "screen hole"?
		return false;

	col = (x % CHARS_PER_LINE);
	uint8_t t = (x / CHARS_PER_LINE);	// "third"
	uint8_t l = (a / 128);			// "line group"
	row = 8*t + l;
	return true;
}

Memory::address Screen::to_address(uint8_t row) {

	return ((row & 7) << 7) | ((row & 24) * 5);
}

void Screen::draw(Memory::address a, uint8_t c) {

	uint8_t row, col;
	if (from_address(a, row, col))
		draw(row, col, c);
}

void Screen::redraw(uint8_t rowstart, uint8_t rowend) {

	DBG_DSP("redraw: %d %d", rowstart, rowend);

	for (uint8_t row = rowstart; row < rowend; row++) {
		Memory::address rowaddr = to_address(row);
		for (uint8_t col = 0; col < CHARS_PER_LINE; col++) {
			draw(row, col, _ram->get(rowaddr + col));
			yield();	// FIXME
		}
	}
}
