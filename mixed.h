#pragma once

class Mixed: public Screen {
public:
	Mixed(Text &t, LoRes &l): text(t), lores(l) {}

	void draw(uint8_t row, uint8_t col, uint8_t c, uint8_t oc);

private:
	Screen &text;
	Screen &lores;
};
