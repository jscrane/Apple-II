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
		Screen &screen = row >= 20? static_cast<Screen&>(text): static_cast<Screen&>(lores);
		screen.draw(addr, b);
	}
}
