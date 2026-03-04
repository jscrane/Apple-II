#pragma once

class Text: public Screen {
public:
	Text(Display &display): Screen(display) {}
	void begin();

protected:
	void draw(Memory::address a, uint8_t c);
};
