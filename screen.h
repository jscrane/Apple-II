#pragma once

class Screen: public Memory::Device {
public:
	static const unsigned N = 1024;

	void show(ram<N> &r) { _ram = &r; }
	void redraw();

	virtual void operator= (uint8_t c) {
		if (c != _ram->get(_acc)) {
			draw(_acc, c);
			_ram->set(_acc, c);
		}
	}
	virtual operator uint8_t () { return _ram->get(_acc); }

	virtual void draw(Memory::address a, uint8_t c) = 0;

protected:
	Screen(): Memory::Device(N), _ram(0) {}

	bool map_address(Memory::address a, uint8_t &row, uint8_t &col);

	ram<N> *_ram;
};
