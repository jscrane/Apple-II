#include <machine.h>
#include <memory.h>
#include <compat.h>
#include <debugging.h>

#include "firmware/applesoft_basic.h"
#include "firmware/autostart_monitor.h"
#include "langcard.h"

LanguageCard::LanguageCard(): Memory::Device(12288), _read_rom(true), _current_bank(_bank0) {
}

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

	if (_acc < 4096)
		return _current_bank[_acc];

	return _main[_acc - 4096];
}

void LanguageCard::Switches::access(uint8_t offset) {

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
