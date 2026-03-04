#pragma once

class SoftSwitches: public Memory::Device {
public:
	void begin();

	virtual void operator= (uint8_t c);
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

	void on_access_speaker(std::function<void(void)> f) { speaker = f; }

	void on_access_graphics_text(std::function<void(bool)> f) { graphics_text = f; }
	void on_access_full_mixed(std::function<void(bool)> f) { full_mixed = f; }
	void on_access_page(std::function<void(bool)> f) { page = f; }
	void on_access_res(std::function<void(bool)> f) { res = f; }

	void on_read_keyboard(std::function<uint8_t(void)> f) { read_keyboard = f; }
	void on_strobe_keyboard(std::function<void(void)> f) { strobe_keyboard = f; }

	bool is_text() const { return text; }
	bool is_mixed() const { return mixed; }
	bool is_page2() const { return page2; }
	bool is_hires() const { return hires; }

private:
	std::function<uint8_t(void)> read_keyboard;
	std::function<void(void)> speaker, strobe_keyboard;
	std::function<void(bool)> graphics_text, full_mixed, page, res;

	bool text, mixed, page2, hires;
};
