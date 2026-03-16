#include <r65emu.h>
#include <r6502.h>
#include <display.h>

#include "config.h"
#include "screen.h"
#include "softswitches.h"
#include "input.h"
#include "disk.h"

Memory memory;
r6502 cpu(memory);
Arduino machine(cpu);
ram<> pages[RAM_PAGES];
flash_filer files(PROGRAMS);
flash_file driveA(1), driveB(2);

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
Screen screen(display);
SoftSwitches switches;
Input input(kbd, files);
Disk disk(memory, driveA, driveB);

#define FLASH_INTERVAL	250000

static void set_screen() {

	static uint8_t last_state;
	bool text = switches.is_text(), mixed = switches.is_mixed();
	bool top_text = text, btm_text = text || mixed;
	uint8_t state = (switches.is_page2() << 2) | (btm_text << 1) | top_text;

	if (state == last_state) return;

	uint8_t diff = state ^ last_state;
	if ((diff & 4) || (diff & 1))
		screen.redraw_top(top_text);

	if ((diff & 4) || (diff & 2))
		screen.redraw_btm(btm_text);
	last_state = state;
}

static void flash_text() {

	static bool flash_is_inverse;

	if (switches.is_text() || switches.is_mixed())
		screen.flash_text(flash_is_inverse);

	flash_is_inverse = !flash_is_inverse;
}

static void file_status() {
	static const char *device_names[MAX_FILES] = { "Tape:", "A:", "B:", "C:", "D:" };
	const char *filename = files.filename();
	display.statusf("%s%s", device_names[files.device()], filename? filename: "No file");
}

static void reset(bool sd) {

	input.reset();

	switches.on_read_keyboard([]() { return input.read(); });
	switches.on_strobe_keyboard([]() { input.strobe(); });

	switches.on_access_page([](bool show_page2) {
		if (show_page2) {
			memory.put(pages[1], 0x0400);
			memory.put(screen, 0x0800);
			screen.show(pages[2]);
		} else {
			screen.show(pages[1]);
			memory.put(screen, 0x0400);
			memory.put(pages[2], 0x0800);
		}
	});
	switches.on_access_graphics_text([](bool) { set_screen(); });
	switches.on_access_full_mixed([](bool) { set_screen(); });

	switches.on_access_speaker([]() { digitalWrite(PWM_SOUND, !digitalRead(PWM_SOUND)); });

	machine.register_cpu_halted_handler([]() {
		disk.on_illegal_instruction();
		cpu.resume();
	});
	machine.register_cpu_debug_handler([]() {
		return (cpu.pc() >= 0xc600 && cpu.pc() < 0xc700) || (cpu.pc() >= 0x9d80 && cpu.pc() < 0x9d90);
	});

	if (!sd) {
		DBG_EMU("No SD Card");
		display.status("No SD Card");
	} else if (!files.start()) {
		DBG_EMU("Failed to open " PROGRAMS);
		display.status("Failed to open " PROGRAMS);
	} else {
#if defined(APPLE_II)
		display.status("Ctrl-B: BASIC");
#else
		file_status();
#endif
	}
}

static void function_key(uint8_t fn) {

	switch (fn) {
	case 1:
		machine.reset();
		return;
	case 2:
		files.advance();
		break;
	case 3:
		files.rewind();
		break;
	case 5:
		input.load();
		return;
	case 8:
		files.next_device();
		break;
	case 10:
		machine.debug_cpu();
		return;
	}
	file_status();
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
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT * Memory::page_size);
#endif

	memory.put(switches, 0xc000);
	memory.put(disk.bootprom, 0xc600);
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
	machine.interval_timer(FLASH_INTERVAL, flash_text);
	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {

	machine.run();
}
