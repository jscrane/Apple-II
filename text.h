#pragma once

class Text: public Screen {
public:
	Text(Display &display): _display(display) {}

	void draw(Memory::address a, uint8_t c);

private:
	Display &_display;
};
