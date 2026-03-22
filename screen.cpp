#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"
#include "softswitches.h"

void Screen::on_mode_change(uint8_t state) {

	if (state == _state) return;

	bool text = (state >> 3) & 1, mixed = (state >> 2) & 1, hgr = state & 1;
	bool top_hgr = !text && hgr, btm_text = text || mixed;

	hires.set_top_active(top_hgr);
	lores.set_top_active(!top_hgr);
	hires.set_btm_active(!btm_text && hgr);
	lores.set_btm_active(btm_text);

	uint8_t diff = state ^ _state;
	if (diff & 0b1011) {	// skip if only "mixed" changed
		if (text)
			lores.redraw(0, SPLIT_LINE, true);
		else if (hgr)
			hires.redraw(0, SPLIT_LINE);
		else
			lores.redraw(0, SPLIT_LINE, false);
	}

	// redraw bottom if any of text/graphics, mixed/full or page2/1 changes
	// _or_ if hires/lores flips _and_ in full-screen graphics mode
	// i.e., skip if in mixed mode and hires/lores changes
	if ((diff & 0b1110) || ((diff & 0x01) && !btm_text)) {
		if (btm_text)
			lores.redraw(SPLIT_LINE, SCREEN_LINES, true);
		else if (hgr)
			hires.redraw(SPLIT_LINE, SCREEN_LINES);
		else
			lores.redraw(SPLIT_LINE, SCREEN_LINES, false);
	}

	_state = state;
}
