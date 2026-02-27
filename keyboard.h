#pragma once

class Keyboard {
public:
	Keyboard(serial_kbd &kbd): _kbd(kbd) {}

	uint8_t read();
	void strobe() { _lastc &= 0x7f; }
	void reset() { _kbd.reset(); }

private:
	uint8_t _lastc;
	serial_kbd &_kbd;
};
