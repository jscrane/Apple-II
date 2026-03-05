#include <stdint.h>

#include <machine.h>
#include <debugging.h>

#include "serialio.h"
#include "filer.h"
#include "serial_kbd.h"
#include "input.h"

void Input::reset() {

	_kbd.reset();
	_loading = false;
}

uint8_t Input::read() {

	if (_lastc & 0x80)
		return _lastc;		// wait for strobe

	int c = -1;
	if (!_loading)
		c = _kbd.read();
	else if (!_files.more())
		_loading = false;
	else
		c = _files.read();

	if (c == -1)
		return _lastc;		// nothing new

	if (c >= 'a' && c <= 'z')
		c &= 0x5f;
	else if (c == '\n')
		c = '\r';

	c |= 0x80;
	_lastc = c;
	DBG_EMU("read: %02x", c);
	return c;
}
