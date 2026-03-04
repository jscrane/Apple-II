#pragma once

class Mixed: public Screen {
public:
	Mixed(Text &t, LoRes &l): text(t), lores(l) {}

	void draw(Memory::address a, uint8_t c);

private:
	Screen &text;
	Screen &lores;
};
