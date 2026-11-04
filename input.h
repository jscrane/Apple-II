#pragma once

#include <fnkey_kbd.h>

class Input: private fnkey_kbd {
public:
	Input(keyboard &kbd, filer &files): fnkey_kbd(kbd), _files(files) {}
	using fnkey_kbd::register_fnkey_handler;

	uint8_t read_kbd();
	void strobe() { _lastc &= 0x7f; }

	void reset() override;
	void load() { _loading = true; }

private:
	uint8_t _lastc;
	filer &_files;
	bool _loading;
};
