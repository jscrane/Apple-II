#pragma once

class Input {
public:
	Input(serial_kbd &kbd, filer &files): _kbd(kbd), _files(files) {}

	uint8_t read();
	void strobe() { _lastc &= 0x7f; }

	void reset();
	void load() { _loading = true; }

private:
	uint8_t _lastc;
	serial_kbd &_kbd;
	filer &_files;
	bool _loading;
};
