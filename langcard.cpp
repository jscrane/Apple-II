#include <machine.h>
#include <memory.h>
#include <compat.h>

#include "firmware/applesoft_basic.h"
#include "firmware/autostart_monitor.h"
#include "langcard.h"

LanguageCard::LanguageCard(): Memory::Device(12288), _read_rom(true) {
}

void LanguageCard::operator=(uint8_t b) {
}

LanguageCard::operator uint8_t() {

	return _acc < 10240? pgm_read_byte(applesoft_basic + _acc): pgm_read_byte(autostart_monitor + _acc - 10240);
}

void LanguageSwitches::operator=(uint8_t b) {
}

LanguageSwitches::operator uint8_t() {
	return 0;
}
