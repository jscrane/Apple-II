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
			DBG_EMU("strobe");
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
	case DISPLAY_TEXT_OFF:
		if (text_mode) {
			text_mode(false);
			return 0;
		}
		break;
	case DISPLAY_TEXT_ON:
		if (text_mode) {
			text_mode(true);
			return 0;
		}
		break;
	case DISPLAY_LORES:
		if (graphics_mode) {
			graphics_mode(false);
			return 0;
		}
		break;
	case DISPLAY_HIRES:
		if (graphics_mode) {
			graphics_mode(true);
			return 0;
		}
		break;
	default:
		//DBG_EMU("SoftSwitches::operator uint8_t unhandled: %04x", _acc);
		return 0;
	}
	DBG_EMU("SoftSwitches::operator uint8_t unregistered handler for: %04x", _acc);
	return 0;
}
