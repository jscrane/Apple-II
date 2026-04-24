#pragma once

class LanguageCard: public Memory::Device {
public:
	LanguageCard();

	operator uint8_t() override;

	void operator=(uint8_t) override;

private:
	bool _read_rom;
};
