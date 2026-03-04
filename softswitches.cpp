#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <debugging.h>

#include "softswitches.h"

void SoftSwitches::operator=(uint8_t c) {
	DBG_EMU("SoftSwitches::operator= %04x %02x", _acc, c);
}

SoftSwitches::operator uint8_t() {

	switch (_acc) {
	case KEYBOARD_DATA:
		if (read_keyboard)
			return read_keyboard();
		break;
	case KEYBOARD_CLEAR_STROBE:
		if (strobe_keyboard) {
			strobe_keyboard();
			return 0;
		}
		break;
	case SPEAKER:
		if (speaker) {
			speaker();
			return 0;
		}
		break;
	case DISPLAY_GRAPHICS:
	case DISPLAY_TEXT:
		if (graphics_text) {
			graphics_text(_acc & 1);
			return 0;
		}
		break;
	case DISPLAY_FULL:
	case DISPLAY_MIXED:
		if (full_mixed) {
			full_mixed(_acc & 1);
			return 0;
		}
		break;
	case DISPLAY_PAGE1:
	case DISPLAY_PAGE2:
		if (page1_page2) {
			page1_page2(_acc & 1);
			return 0;
		}
		break;
	case DISPLAY_LORES:
	case DISPLAY_HIRES:
		if (lores_hires) {
			lores_hires(_acc & 1);
			return 0;
		}
		break;
	default:
		DBG_EMU("SoftSwitches::operator uint8_t unhandled: %02x", _acc);
		return 0;
	}
	DBG_EMU("SoftSwitches::operator uint8_t unregistered handler for: %02x", _acc);
	return 0;
}
