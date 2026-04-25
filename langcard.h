#pragma once

class LanguageCard: public Memory::Device {
public:
	LanguageCard();

	operator uint8_t() override;

	void operator=(uint8_t) override;

private:
	bool _read_rom;
};

class LanguageSwitches: public Memory::Device {
public:
	LanguageSwitches(LanguageCard &card): Memory::Device(16), _card(card) {}

	operator uint8_t() override;

	void operator=(uint8_t) override;

private:
	LanguageCard _card;
};
