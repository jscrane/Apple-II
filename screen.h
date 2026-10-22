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

	void operator=(uint8_t c) override {
		_ram->set(_acc, c);
		if (_top_active || _btm_active) on_set(c);
	}

	operator uint8_t() override { return _ram->get(_acc); }

	void checkpoint(Checkpoint &c) override {
		_ram->checkpoint(c);
		c.write(_top_active);
		c.write(_btm_active);
	}

	void restore(Checkpoint &c) override {
		_ram->restore(c);
		c.read(_top_active);
		c.read(_btm_active);
	}

protected:
	Resolution(): Memory::Device(N) {}

	virtual void on_page_change() {}

	virtual void on_set(uint8_t) =0;

	ram<N> *_ram;

	bool _top_active, _btm_active;
};

class Lores: public Resolution<1024> {
public:
	using Base = Resolution<1024>;
	using Base::operator=;

	Lores(Display &display): _display(display) {}
	void flash_text(bool flash_is_inverse);
	void redraw(uint8_t rowstart, uint8_t rowend, bool as_text);

	void checkpoint(Checkpoint &c) override;
	void restore(Checkpoint &c) override;

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
	using Base = Resolution<8192>;
	using Base::operator=;

	Hires(Display &display): _display(display) {}
	void redraw(uint8_t rowstart, uint8_t rowend);

	void checkpoint(Checkpoint &c) override;
	void restore(Checkpoint &c) override;

private:
	void on_set(uint8_t) override;
	bool from_address(Memory::address a, uint16_t &x, uint16_t &y);
	Memory::address to_address(uint16_t y);

	// mono
	void draw(uint8_t b, uint16_t x, uint16_t y);

	// colour
	void redraw_row(uint16_t y);
	void redraw_dirty();

	Display &_display;
};

class Screen {
public:
	Screen(Display &display): lores(display), hires(display), _mode(0) {}

	Lores lores;
	Hires hires;

	void on_mode_change(uint8_t mode);
	void redraw(uint8_t mode);

private:
	uint8_t _mode;
};
