#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <ram.h>
#include <display.h>

#include "screen.h"
#include "text.h"
#include "lores.h"
#include "mixed.h"

void Mixed::draw(Memory::address addr, uint8_t b) {

	uint8_t row, col;
	if (map_address(addr, row, col)) {
		if (row >= 20)
			text.draw(addr, b);
		else
			lores.draw(addr, b);
	}
}
