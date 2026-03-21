#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"

void Hires::on_page_change() {
	// FIXME
	DBG_DSP("Hires::on_page_change");
}

void Hires::on_set(uint8_t b) {
	// FIXME
	DBG_DSP("Hires::on_set");
}

void Hires::redraw(uint8_t rowstart, uint8_t rowend) {
	// FIXME
	DBG_DSP("Hires::redraw %d %d", rowstart, rowend);
}
