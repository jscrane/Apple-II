#include <stdint.h>

#include <machine.h>
#include <debugging.h>

#include "serial_kbd.h"
#include "keyboard.h"

uint8_t Keyboard::read() {
	if (_kbd.available()) {
		uint8_t c = _kbd.read();
		if (c >= 'a' && c <= 'z')
			c &= 0x5f;
		c |= 0x80;
		_lastc = c;
		DBG_EMU("read: %02x", c);
		return c;
	}
	return _lastc;
}
