#pragma once

class LoRes: public Screen {
public:
	LoRes(Display &display): _display(display) {}

	void draw(uint8_t row, uint8_t col, uint8_t c);

private:
	Display &_display;
};
