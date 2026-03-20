#pragma once

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define SPLIT_LINE	20
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

template<unsigned N> class Resolution: public Memory::Device {
public:
	void show_page(ram<N> &r) { _ram = &r; on_page_change(); }

	virtual void operator=(uint8_t c) { if (c != _ram->get(_acc)) { _ram->set(_acc, c); on_set(c); } }

	virtual operator uint8_t() { return _ram->get(_acc); }

protected:
	Resolution(): Memory::Device(N) {}

	virtual void on_page_change() =0;

	virtual void on_set(uint8_t c) =0;

	ram<N> *_ram;
};

class Screen: public Resolution<1024> {
public:
	Screen(Display &display): _display(display) {}

	void redraw_top(bool as_text) { _top_text = as_text; redraw(0, SPLIT_LINE, as_text); }

	void redraw_btm(bool as_text) { _btm_text = as_text; redraw(SPLIT_LINE, SCREEN_LINES, as_text); }

	void flash_text(bool flash_is_inverse);
private:
	void on_page_change() override;

	void on_set(uint8_t) override;

	void redraw(uint8_t, uint8_t, bool);

	void draw_text(uint8_t, uint8_t, uint8_t);

	void draw_lores(uint8_t, uint8_t, uint8_t);

	bool from_address(Memory::address a, uint8_t &row, uint8_t &col);

	Memory::address to_address(uint8_t row);

	uint32_t _flashrows;

	Display &_display;

	bool _top_text, _btm_text;
};
