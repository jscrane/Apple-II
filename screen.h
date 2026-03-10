#pragma once

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

class Screen: public Memory::Device {
public:
	static const unsigned N = 1024;

	void show(ram<N> &r) { _ram = &r; }
	void redraw(uint8_t rowstart, uint8_t rowend);

	virtual void operator=(uint8_t c) {
		if (c != _ram->get(_acc)) {
			draw(_acc, c);
			_ram->set(_acc, c);
		}
	}
	virtual operator uint8_t() { return _ram->get(_acc); }

	virtual void draw(uint8_t row, uint8_t col, uint8_t c, uint8_t oc) = 0;

protected:
	Screen(): Memory::Device(N), _ram(0) {}

	bool from_address(Memory::address a, uint8_t &row, uint8_t &col);

	Memory::address to_address(uint8_t row);

	void draw(Memory::address a, uint8_t c);

	ram<N> *_ram;
};
