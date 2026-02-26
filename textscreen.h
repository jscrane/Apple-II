#pragma once

class TextScreen: public Memory::Device {
public:
	TextScreen(Display &display): Memory::Device(sizeof(_mem)), _display(display) {}
	void begin();
	void enable(bool on) { _on = on; }

	virtual void operator= (uint8_t c) { _set(_acc, c); }
	virtual operator uint8_t () { return _mem[_acc]; }

private:
	void _set(Memory::address a, uint8_t c);
	uint8_t _mem[1024];
	bool _on;
	Display &_display;
};
