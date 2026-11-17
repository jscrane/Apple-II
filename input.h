#pragma once

#include <fnkey_kbd.h>

class flash_file;

class Input: private fnkey_kbd {
public:
	Input(keyboard &kbd, flash_file &tape): fnkey_kbd(kbd), _tape(tape) {}
	using fnkey_kbd::register_fnkey_handler;

	uint8_t read_kbd();
	void strobe() { _lastc &= 0x7f; }

	void reset() override;
	void load() { _loading = true; }

private:
	flash_file &_tape;
	uint8_t _lastc = 0;
	bool _loading = false;
};
