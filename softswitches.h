#pragma once

class SoftSwitches: public Memory::Device {
public:
	SoftSwitches(): Memory::Device(128) {}
	void begin();

	virtual void operator= (uint8_t) { toggle(); }
	virtual operator uint8_t ();

	static const uint8_t KEYBOARD_DATA = 0x00;
	static const uint8_t KEYBOARD_CLEAR_STROBE = 0x10;

	static const uint8_t SPEAKER = 0x30;

	static const uint8_t DISPLAY_GRAPHICS = 0x50;
	static const uint8_t DISPLAY_TEXT = 0x51;
	static const uint8_t DISPLAY_FULL = 0x52;
	static const uint8_t DISPLAY_MIXED = 0x53;
	static const uint8_t DISPLAY_PAGE1 = 0x54;
	static const uint8_t DISPLAY_PAGE2 = 0x55;
	static const uint8_t DISPLAY_LORES = 0x56;
	static const uint8_t DISPLAY_HIRES = 0x57;

	// "annunciators", pins 15-12 of the game i/o socket
	// (these can be connected to gpios via handlers)
	static const uint8_t AN0_OFF = 0x58;
	static const uint8_t AN0_ON = 0x59;
	static const uint8_t AN1_OFF = 0x5a;
	static const uint8_t AN1_ON = 0x5b;
	static const uint8_t AN2_OFF = 0x5c;
	static const uint8_t AN2_ON = 0x5d;
	static const uint8_t AN3_OFF = 0x5e;
	static const uint8_t AN3_ON = 0x5f;

	void on_read_keyboard(std::function<uint8_t(void)> f) { read_keyboard = f; }
	void on_strobe_keyboard(std::function<void(void)> f) { strobe_keyboard = f; }

	void on_access_speaker(std::function<void(void)> f) { speaker = f; }

	void on_access_graphics_text(std::function<void(bool)> f) { graphics_text = f; }
	void on_access_full_mixed(std::function<void(bool)> f) { full_mixed = f; }
	void on_access_page(std::function<void(bool)> f) { page = f; }
	void on_access_res(std::function<void(bool)> f) { res = f; }

	void on_access_an0(std::function<void(bool)> f) { an0 = f; }
	void on_access_an1(std::function<void(bool)> f) { an1 = f; }
	void on_access_an2(std::function<void(bool)> f) { an2 = f; }
	void on_access_an3(std::function<void(bool)> f) { an3 = f; }

	bool is_text() const { return text; }
	bool is_mixed() const { return mixed; }
	bool is_page2() const { return page2; }
	bool is_hires() const { return hires; }

private:
	void toggle();

	std::function<uint8_t(void)> read_keyboard;
	std::function<void(void)> speaker, strobe_keyboard;
	std::function<void(bool)> graphics_text, full_mixed, page, res;
	std::function<void(bool)> an0, an1, an2, an3;

	bool text, mixed, page2, hires;
};
