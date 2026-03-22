#pragma once

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define SPLIT_LINE	20
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

template<unsigned N> class Resolution: public Memory::Device {
public:
	void show_page(ram<N> &r) { _ram = &r; on_page_change(); }

	void set_top_active(bool a) { _top_active = a; }

	void set_btm_active(bool a) { _btm_active = a; }

	virtual void operator=(uint8_t c) { _ram->set(_acc, c); if (_top_active || _btm_active) on_set(c); }

	virtual operator uint8_t() { return _ram->get(_acc); }
protected:
	Resolution(): Memory::Device(N) {}

	virtual void on_page_change() {}

	virtual void on_set(uint8_t c) =0;

	ram<N> *_ram;

	bool _top_active, _btm_active;
};

class Lores: public Resolution<1024> {
public:
	Lores(Display &display): _display(display) {}

	void flash_text(bool flash_is_inverse);

	void redraw(uint8_t rowstart, uint8_t rowend, bool as_text);
private:
	void on_page_change() override;

	void on_set(uint8_t) override;

	void draw_text(uint8_t, uint8_t, uint8_t);

	void draw_lores(uint8_t, uint8_t, uint8_t);

	bool from_address(Memory::address a, uint8_t &row, uint8_t &col);

	Memory::address to_address(uint8_t row);

	uint32_t _flashrows;

	Display &_display;

	bool _top_text, _btm_text;
};

class Hires: public Resolution<8192> {
public:
	Hires(Display &display): _display(display) {}

	void redraw(uint8_t rowstart, uint8_t rowend);
private:
	void on_set(uint8_t b) override;

	bool from_address(Memory::address a, uint16_t &x, uint16_t &y);

	Memory::address to_address(uint16_t y);

	void draw(uint8_t b, uint16_t x, uint16_t y);

	Display &_display;
};

class SoftSwitches;

class Screen {
public:
	Screen(Display &display): lores(display), hires(display), _mode(0) {}

	Lores lores;

	Hires hires;

	void on_mode_change(uint8_t mode);

private:
	uint8_t _mode;
};
