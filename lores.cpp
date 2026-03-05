#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <ram.h>

#include "screen.h"
#include "lores.h"

static uint16_t colours[] = {
	0x0000, 0xc197, 0x0014, 0xc1ff, 0x0029, 0x8410, 0x0019, 0x24ff,
	0x8200, 0xeb20, 0x8410, 0xf39d, 0x07e0, 0xffe0, 0x07ff, 0xffff
};

void LoRes::draw(Memory::address a, uint8_t b) {

	uint8_t row, col;
	if (map_address(a, row, col)) {
		_display.fillRect(CHAR_WIDTH*col+1, CHAR_HEIGHT*row, CHAR_WIDTH, CHAR_HEIGHT / 2, colours[b & 0x0f]);
		_display.fillRect(CHAR_WIDTH*col+1, CHAR_HEIGHT*row+4, CHAR_WIDTH, CHAR_HEIGHT / 2, colours[b >> 4]);
	}
}
