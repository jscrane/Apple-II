#pragma once

class SoftSwitches: public Memory::Device {
public:
	void begin();

	virtual void operator= (uint8_t c);
	virtual operator uint8_t ();

	static const uint8_t KEYBOARD_DATA = 0x00;
	static const uint8_t KEYBOARD_CLEAR_STROBE = 0x10;

	static const uint8_t SPEAKER = 0x30;

	static const uint8_t DISPLAY_TEXT_OFF = 0x50;	// i.e., Graphics on
	static const uint8_t DISPLAY_TEXT_ON = 0x51;
	static const uint8_t DISPLAY_FULL = 0x52;
	static const uint8_t DISPLAY_MIXED = 0x53;
	static const uint8_t DISPLAY_PAGE1 = 0x54;
	static const uint8_t DISPLAY_PAGE2 = 0x55;
	static const uint8_t DISPLAY_LORES = 0x56;
	static const uint8_t DISPLAY_HIRES = 0x57;

	void on_access_speaker(std::function<void(void)> f) { speaker = f; }
	void on_access_text(std::function<void(bool)> f) { text_mode = f; }
	void on_access_graphics(std::function<void(bool)> f) { graphics_mode = f; }
	void on_read_keyboard(std::function<uint8_t(void)> f) { read_keyboard = f; }
	void on_strobe_keyboard(std::function<void(void)> f) { strobe_keyboard = f; }

private:
	std::function<uint8_t(void)> read_keyboard;
	std::function<void(void)> speaker, strobe_keyboard;
	std::function<void(bool)> text_mode, graphics_mode;
};
