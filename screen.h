#pragma once

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define SPLIT_LINE	20
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

class Screen: public Memory::Device {
public:
	static const unsigned N = 1024;

	Screen(Display &display): Memory::Device(N), _display(display) {}
	void show(ram<N> &r);

	virtual void operator=(uint8_t c) { if (c != _ram->get(_acc)) set(c); }
	virtual operator uint8_t() { return _ram->get(_acc); }

	void redraw_top(bool as_text) { _top_text = as_text; redraw(0, SPLIT_LINE, as_text); }
	void redraw_btm(bool as_text) { _btm_text = as_text; redraw(SPLIT_LINE, SCREEN_LINES, as_text); }

	void flash_text(bool flash_is_inverse);

private:
	void redraw(uint8_t, uint8_t, bool);

	void draw_text(uint8_t, uint8_t, uint8_t);

	void draw_lores(uint8_t, uint8_t, uint8_t);

	void set(uint8_t);

	bool from_address(Memory::address a, uint8_t &row, uint8_t &col);

	Memory::address to_address(uint8_t row);

	ram<N> *_ram;

	uint32_t _flashrows;

	Display &_display;

	bool _top_text, _btm_text;
};
