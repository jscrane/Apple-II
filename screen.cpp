#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <display.h>
#include <debugging.h>
#include <ram.h>

#include "screen.h"
#include "softswitches.h"

void Screen::on_mode_change() {

	bool text = _switches.is_text(), mixed = _switches.is_mixed(), hgr = _switches.is_hires();
	bool top_text = text && !hgr, btm_text = text || mixed;
	uint8_t state = (text << 4) | (hgr << 3) | (_switches.is_page2() << 2) | (btm_text << 1) | top_text;

	if (state == _state) return;

	DBG_DSP("text=%d mixed=%d hgr=%d", text, mixed, hgr);

	if (text) {
		lores.set_top_active(true);
		lores.set_btm_active(true);
		hires.set_top_active(false);
		hires.set_btm_active(false);
	} else if (mixed) {
		lores.set_top_active(!hgr);
		lores.set_btm_active(true);
		hires.set_top_active(hgr);
		hires.set_btm_active(false);
	} else {
		lores.set_top_active(!hgr);
		lores.set_btm_active(!hgr);
		hires.set_top_active(hgr);
		hires.set_btm_active(hgr);
	}

	uint8_t diff = state ^ _state;
	if ((diff & 16) || (diff & 8) || (diff & 4) || (diff & 1)) {
		if (top_text)
			lores.redraw(0, SPLIT_LINE, true);
		else if (hgr)
			hires.redraw(0, SPLIT_LINE);
		else
			lores.redraw(0, SPLIT_LINE, false);
	}

	if ((diff & 16) || (diff & 8) || (diff & 4) || (diff & 2)) {
		if (btm_text)
			lores.redraw(SPLIT_LINE, SCREEN_LINES, true);
		else if (hgr)
			hires.redraw(SPLIT_LINE, SCREEN_LINES);
		else
			lores.redraw(SPLIT_LINE, SCREEN_LINES, false);
	}

	_state = state;
}

