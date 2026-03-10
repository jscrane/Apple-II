#pragma once

class Text: public Screen {
public:
	Text(Display &display): _display(display) {}

	void draw(uint8_t row, uint8_t col, uint8_t c, uint8_t oc);

private:
	Display &_display;
};
