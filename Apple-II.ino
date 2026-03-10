#include <r65emu.h>
#include <r6502.h>
#include <display.h>

#include "config.h"
#include "screen.h"
#include "text.h"
#include "lores.h"
#include "mixed.h"
#include "softswitches.h"
#include "input.h"

Memory memory;
r6502 cpu(memory);
Arduino machine(cpu);
ram<> pages[RAM_PAGES];
flash_filer files(PROGRAMS);

#if defined(APPLE_II)
#include "firmware/original_monitor.h"
#include "firmware/integer_basic1.h"
#include "firmware/integer_basic2.h"
#include "firmware/integer_basic3.h"
prom monitor(original_monitor, sizeof(original_monitor));
prom basic1(integer_basic1, sizeof(integer_basic1));
prom basic2(integer_basic2, sizeof(integer_basic2));
prom basic3(integer_basic3, sizeof(integer_basic3));
#elif defined(APPLE_II_PLUS)
#include "firmware/autostart_monitor.h"
#include "firmware/applesoft_basic1.h"
#include "firmware/applesoft_basic2.h"
#include "firmware/applesoft_basic3.h"
#include "firmware/applesoft_basic4.h"
#include "firmware/applesoft_basic5.h"
prom monitor(autostart_monitor, sizeof(autostart_monitor));
prom basic1(applesoft_basic1, sizeof(applesoft_basic1));
prom basic2(applesoft_basic2, sizeof(applesoft_basic2));
prom basic3(applesoft_basic3, sizeof(applesoft_basic3));
prom basic4(applesoft_basic4, sizeof(applesoft_basic4));
prom basic5(applesoft_basic5, sizeof(applesoft_basic5));
#endif

#if defined(USE_HOST_KBD)
hw_serial_kbd kbd(Serial);
#elif defined(USE_PS2_KBD)
ps2_serial_kbd kbd;
#else
#error "No keyboard defined!"
#endif

Display display;
Text text(display);
LoRes lores(display);
Mixed mixed(text, lores);
SoftSwitches switches;
Input input(kbd, files);

static Screen &get_active_screen() {
	if (switches.is_text()) return text;
	if (switches.is_mixed()) return mixed;
	return lores;
}

static void set_screen() {
	static Screen *last;
	static bool was_page2;

	Screen &screen = get_active_screen();
	memory.put(screen, switches.is_page2()? 0x0800: 0x0400);

	if (was_page2 != switches.is_page2()) {
		screen.redraw(0, 24);
		was_page2 = switches.is_page2();
		last = &screen;
		return;
	}
	if (last == &screen)
		return;

	uint8_t rs = 0, re = 24;
	if (last == &text)
		re = 20;
	else if (last == &lores)
		rs = switches.is_text()? 0: 20;
	else if (switches.is_text())
		re = 20;
	else
		rs = 20;

	screen.redraw(rs, re);
	last = &screen;
}

static void reset(bool sd) {

	input.reset();

	switches.on_read_keyboard([]() { return input.read(); });
	switches.on_strobe_keyboard([]() { input.strobe(); });

	switches.on_access_page([](bool show_page2) {
		if (show_page2) {
			memory.put(pages[1], 0x0400);
			text.show(pages[2]);
			lores.show(pages[2]);
			mixed.show(pages[2]);
		} else {
			text.show(pages[1]);
			lores.show(pages[1]);
			mixed.show(pages[1]);
			memory.put(pages[2], 0x0800);
		}
	});
	switches.on_access_graphics_text([](bool) { set_screen(); });
	switches.on_access_full_mixed([](bool) { set_screen(); });

	switches.on_access_speaker([]() { digitalWrite(PWM_SOUND, !digitalRead(PWM_SOUND)); });

	if (!sd) {
		DBG_EMU("No SD Card");
		display.status("No SD Card");
	} else if (!files.start()) {
		DBG_EMU("Failed to open " PROGRAMS);
		display.status("Failed to open " PROGRAMS);
	} else {
#if defined(APPLE_II)
		display.status("Ctrl-B: BASIC");
#endif
	}
}

static const char *open(const char *filename) {
	if (filename) {
		display.status(filename);
		return filename;
	}
	display.status("No file");
	return 0;
}

static void function_key(uint8_t fn) {
	static const char *filename;

	switch (fn) {
	case 1:
		machine.reset();
		break;
	case 2:
		filename = open(files.advance());
		break;
	case 3:
		filename = open(files.rewind());
		break;
	case 5:
		input.load();
		break;
	case 10:
		machine.debug_cpu();
		break;
	}
}

void setup() {

	machine.begin();

	display.begin(BG_COLOUR, FG_COLOUR, ORIENT);
	display.setScreen(CHAR_WIDTH * CHARS_PER_LINE, CHAR_HEIGHT * SCREEN_LINES);
	display.clear();

	DBG_INI("RAM:    %dkB at 0x0000", RAM_PAGES);
	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], i * ram<>::page_size);

#if defined(USE_SPIRAM)
	DBG_INI("SpiRAM: %dkB at 0x%04x", SPIRAM_EXTENT * Memory::page_size / 1024, SPIRAM_BASE);
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#endif

	memory.put(switches, 0xc000);
	memory.put(monitor, 0xf800);

#if defined(APPLE_II)
	memory.put(basic1, 0xe000);
	memory.put(basic2, 0xe800);
	memory.put(basic3, 0xf000);
#elif defined(APPLE_II_PLUS)
	memory.put(basic1, 0xd000);
	memory.put(basic2, 0xd800);
	memory.put(basic3, 0xe000);
	memory.put(basic4, 0xe800);
	memory.put(basic5, 0xf000);
#endif

	kbd.register_fnkey_handler(function_key);
	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {

	machine.run();
}
