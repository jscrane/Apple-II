#pragma once

class LanguageCard: public Memory::Device {
public:
	LanguageCard(): Memory::Device(12288), _read_rom(true), _current_bank(_bank2) {}

	operator uint8_t() override;
	void operator=(uint8_t) override;

	class Switches: public Memory::Device {
	public:
		Switches(LanguageCard &card): Memory::Device(16), _card(card), _write_pending(false) {}

		operator uint8_t() override { any_access(_acc & 0x0f); return 0; }
		void operator=(uint8_t) override { any_access(_acc & 0x0f); }

	private:
		void any_access(uint8_t);

		LanguageCard &_card;
		bool _write_pending;
	};

private:
	bool _read_rom = true, _can_write = false;
	uint8_t *_current_bank;

	void select_bank(bool bank1) { _current_bank = bank1? _bank1: _bank2; }
	void read_rom() { _read_rom = true; }
	void read_ram() { _read_rom = false; }
	void write_enable(bool enable) { _can_write = enable; }

	// currently this just about fits in ram on an esp8266
	uint8_t _main[8192];
	uint8_t _bank1[4096], _bank2[4096];
};
