#pragma once

class Text: public Screen {
public:
	Text(Display &display): _display(display) {}
	virtual void operator=(uint8_t);

	void draw(uint8_t row, uint8_t col, uint8_t c, uint8_t oc);
	void flash(uint8_t rowstart, uint8_t rowend, bool flash_is_inverse);

private:
	Display &_display;
	uint32_t _flashrows;
};
