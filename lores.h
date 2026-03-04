#pragma once

class LoRes: public Screen {
public:
	LoRes(Display &display): Screen(display) {}

protected:
	void draw(Memory::address a, uint8_t c);
};
