#pragma once

class Mixed: public Screen {
public:
	Mixed(Text &t, LoRes &l): text(t), lores(l) {}

	void draw(uint8_t row, uint8_t col, uint8_t c);

private:
	Screen &text;
	Screen &lores;
};
