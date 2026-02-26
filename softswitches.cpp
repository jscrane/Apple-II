#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <debugging.h>

#include "softswitches.h"

void SoftSwitches::operator=(uint8_t c) {
	DBG_EMU("SoftSwitches::operator= %04x %02x", _acc, c);
}

SoftSwitches::operator uint8_t() {

	DBG_EMU("SoftSwitches::operator uint8_t %04x", _acc);

	switch (_acc) {
	case DISPLAY_TEXT_OFF:
		if (text_mode)
			text_mode(false);
		break;
	case DISPLAY_TEXT_ON:
		if (text_mode)
			text_mode(true);
		break;
	case DISPLAY_LORES:
		if (graphics_mode)
			graphics_mode(false);
		break;
	case DISPLAY_HIRES:
		if (graphics_mode)
			graphics_mode(true);
		break;
	}
	return 0x00;
}
