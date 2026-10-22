#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <compat.h>
#include <debugging.h>

#include "firmware/applesoft_basic.h"
#include "firmware/autostart_monitor.h"
#include "langcard.h"

void LanguageCard::operator=(uint8_t b) {

	DBG_MEM("write: %04x %d %d", _acc, _can_write, b);

	if (!_can_write)
		return;

	if (_acc < 4096)
		_current_bank[_acc] = b;
	else
		_main[_acc - 4096] = b;
}

LanguageCard::operator uint8_t() {

	DBG_MEM("read: %04x", _acc);

	if (_read_rom)
		return _acc < 10240? pgm_read_byte(applesoft_basic + _acc): pgm_read_byte(autostart_monitor + _acc - 10240);

	return _acc < 4096? _current_bank[_acc]: _main[_acc - 4096];
}

void LanguageCard::Switches::any_access(uint8_t offset) {

	DBG_MEM("access: %02x", offset);

	_card.select_bank(offset & 0x08);

	switch (offset & 0x03) {
	case 0:
		_card.read_ram();
		_card.write_enable(false);
		break;
	case 1:
		_card.read_rom();
		if (_write_pending)
			_card.write_enable(true);
		break;
	case 2:
		_card.read_rom();
		_card.write_enable(false);
		break;
	case 3:
		_card.read_ram();
		if (_write_pending)
			_card.write_enable(true);
		break;
	}
	_write_pending = (offset & 0x01);
}

void LanguageCard::checkpoint(Checkpoint &c) {

	DBG_EMU("langcard: checkpoint: %d", c.pos());
	c.write(_read_rom);
	c.write(_can_write);
	c.write(_current_bank == _bank1);
	c.write(_main, sizeof(_main));
	c.write(_bank1, sizeof(_bank1));
	c.write(_bank2, sizeof(_bank2));
}

void LanguageCard::restore(Checkpoint &c) {

	DBG_EMU("langcard: restore: %d", c.pos());
	c.read(_read_rom);
	c.read(_can_write);
	bool bank1;
	c.read(bank1);
	select_bank(bank1);
	c.read(_main, sizeof(_main));
	c.read(_bank1, sizeof(_bank1));
	c.read(_bank2, sizeof(_bank2));
}
