#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "config.h"
#include "screen.h"

bool Screen::map_address(Memory::address a, uint8_t &row, uint8_t &col) {

	uint8_t x = (a % 128);
	if (x >= 120)	// "screen hole"?
		return false;

	col = (x % CHARS_PER_LINE);
	uint8_t t = (x / CHARS_PER_LINE);	// "third"
	uint8_t l = (_acc / 128);		// "line group"
	row = 8*t + l;
	return true;
}

void Screen::redraw() {

	_display.clear();
	for (uint16_t addr = 0; addr < N; addr++) {
		uint8_t c = _ram->get(addr);
		_ram->set(addr, 0);
		draw(addr, c);
	}
}
