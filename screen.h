#pragma once

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define SPLIT_LINE	20
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

inline bool is_flash(uint8_t b) { return b >= 0x40 && b < 0x80; }

inline bool is_inverse(uint8_t b) { return b < 0x40; }

class Screen: public Memory::Device {
public:
	static const unsigned N = 1024;
	static void show(ram<N> &r);

	virtual void operator=(uint8_t c) { if (c != _ram->get(_acc)) set(_acc, c); }
	virtual operator uint8_t() { return _ram->get(_acc); }

	virtual void draw(uint8_t row, uint8_t col, uint8_t c) = 0;
	void redraw(uint8_t rowstart, uint8_t rowend);

protected:
	Screen(): Memory::Device(N) {}

	void set(Memory::address a, uint8_t c);

	void draw(Memory::address a, uint8_t c);

	static bool from_address(Memory::address a, uint8_t &row, uint8_t &col);

	static Memory::address to_address(uint8_t row);

	static ram<N> *_ram;

	static uint32_t _flashrows;
};
