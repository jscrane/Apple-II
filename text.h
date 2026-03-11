#pragma once

class Text: public Screen {
public:
	Text(Display &display): _display(display) {}

	void draw(uint8_t row, uint8_t col, uint8_t c);
	void flash(uint8_t rowstart, uint8_t rowend, bool flash_is_inverse);

private:
	Display &_display;
};
