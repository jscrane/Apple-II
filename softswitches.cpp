#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <debugging.h>

#include "softswitches.h"

void SoftSwitches::toggle() {

	switch (_acc) {
	case KEYBOARD_CLEAR_STROBE:
		if (strobe_keyboard) {
			strobe_keyboard();
			return;
		}
		break;
	case SPEAKER:
		if (speaker) {
			speaker();
			return;
		}
		break;
	case DISPLAY_GRAPHICS:
	case DISPLAY_TEXT:
		text = (_acc & 1);
		if (graphics_text) {
			graphics_text(text);
			return;
		}
		break;
	case DISPLAY_FULL:
	case DISPLAY_MIXED:
		mixed = (_acc & 1);
		if (full_mixed) {
			full_mixed(mixed);
			return;
		}
		break;
	case DISPLAY_PAGE1:
	case DISPLAY_PAGE2:
		page2 = (_acc & 1);
		if (page) {
			page(page2);
			return;
		}
		break;
	case DISPLAY_LORES:
	case DISPLAY_HIRES:
		hires = (_acc & 1);
		if (res) {
			res(hires);
			return;
		}
		break;
	case AN0_OFF:
	case AN0_ON:
		if (an0) {
			an0(_acc & 1);
			return;
		}
		break;
	case AN1_OFF:
	case AN1_ON:
		if (an1) {
			an1(_acc & 1);
			return;
		}
		break;
	case AN2_OFF:
	case AN2_ON:
		if (an2) {
			an2(_acc & 1);
			return;
		}
		break;
	case AN3_OFF:
	case AN3_ON:
		if (an3) {
			an3(_acc & 1);
			return;
		}
		break;
	default:
		DBG_EMU("SoftSwitches::toggle unhandled: %02x", _acc);
		return;
	}
	DBG_EMU("SoftSwitches::toggle unregistered handler for: %02x", _acc);
}

SoftSwitches::operator uint8_t() {

	switch (_acc) {
	case KEYBOARD_DATA:
		if (read_keyboard)
			return read_keyboard();
		break;
	default:
		toggle();
		return 0;
	}
	DBG_EMU("SoftSwitches::operator uint8_t unregistered handler for: %02x", _acc);
	return 0;
}
