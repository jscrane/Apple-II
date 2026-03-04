#pragma once

class LoRes: public Screen {
public:
	LoRes(Display &display): _display(display) {}

	void draw(Memory::address a, uint8_t c);

private:
	Display &_display;
};
