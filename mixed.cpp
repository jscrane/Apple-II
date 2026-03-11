#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <ram.h>
#include <display.h>

#include "screen.h"
#include "text.h"
#include "lores.h"
#include "mixed.h"

void Mixed::draw(uint8_t row, uint8_t col, uint8_t c) {

	if (row < 20)
		lores.draw(row, col, c);
	else
		text.draw(row, col, c);
}
