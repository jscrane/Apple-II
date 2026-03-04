#pragma once

class TextScreen: public Memory::Device {
public:
	static const unsigned N = 1024;

	TextScreen(Display &display): Memory::Device(N), _display(display), _ram(0) {}
	void begin();

	void display(ram<N> &r) { _ram = &r; }
	void redraw();

	virtual void operator= (uint8_t c) { _set(_acc, c); }
	virtual operator uint8_t () { return _ram->get(_acc); }

private:
	void _set(Memory::address a, uint8_t c);
	Display &_display;
	ram<N> *_ram;
};
