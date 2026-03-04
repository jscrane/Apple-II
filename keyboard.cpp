#include <stdint.h>

#include <machine.h>
#include <debugging.h>

#include "serial_kbd.h"
#include "keyboard.h"

uint8_t Keyboard::read() {
	int c = _kbd.read();
	if (c == -1)
		return _lastc;

	if (c >= 'a' && c <= 'z')
		c &= 0x5f;
	else if (c == '\b')
		c = 0x08;

	c |= 0x80;
	_lastc = c;
	DBG_EMU("read: %02x", c);
	return c;
}
