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
ram<1024> sys[8];
ram<1024> &lgr_page1 = sys[1], &lgr_page2 = sys[2];
ram<8192> hgr_page1, hgr_page2;
#if defined(USE_SPIRAM)
spiram::Block user(sram, 24576);
#else
ram<24576> user;
#endif
flash_filer files(PROGRAMS);
flash_file drive1(1), drive2(2);

#if defined(APPLE_II)
#include "firmware/original_monitor.h"
#include "firmware/integer_basic.h"
prom monitor(original_monitor, sizeof(original_monitor));
prom language(integer_basic, sizeof(integer_basic));
Memory::address langaddr = 0xe000;

#elif defined(APPLE_II_PLUS)
#include "firmware/autostart_monitor.h"
#include "firmware/applesoft_basic.h"
prom monitor(autostart_monitor, sizeof(autostart_monitor));
prom language(applesoft_basic, sizeof(applesoft_basic));
Memory::address langaddr = 0xd000;

#elif defined(LANGUAGE_CARD)
#include "langcard.h"
LanguageCard language;
LanguageCard::Switches lang_switches(language);
Memory::address langaddr = 0xd000;
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
Memory::Devices systemio;
Input input(kbd, files);
DiskII disk(memory, drive1, drive2);
DiskII::Switches disk_switches(cpu, disk);

#define FLASH_INTERVAL	250000

static void screen_mode_change() {
	screen.on_mode_change(switches.screen_mode());
}

// Lores watches memory regions $400 or $800 depending on page
// Hires watches memory regions $2000 or $4000 depending on page
// (because of mixed mode, additional guards are needed to prevent
// drawing at the wrong time: see screen_mode_change)
static void screen_page_change() {

	memory.put(lgr_page1, 0x0400);
	memory.put(lgr_page2, 0x0800);
	memory.put(hgr_page1, 0x2000);
	memory.put(hgr_page2, 0x4000);

	if (switches.is_page2()) {
		memory.put(screen.hires, 0x4000);
		screen.hires.show_page(hgr_page2);
		memory.put(screen.lores, 0x0800);
		screen.lores.show_page(lgr_page2);
	} else {
		memory.put(screen.hires, 0x2000);
		screen.hires.show_page(hgr_page1);
		memory.put(screen.lores, 0x0400);
		screen.lores.show_page(lgr_page1);
	}
	screen_mode_change();
}

static void flash_text() {

	static bool flash_is_inverse;

	if (switches.is_text() || switches.is_mixed())
		screen.lores.flash_text(flash_is_inverse);

	flash_is_inverse = !flash_is_inverse;
}

static void file_status() {

	static const char *device_names[MAX_FILES] = { "Tape:", "D1:", "D2:" };
	const char *filename = files.filename();
	display.statusf("%s%s", device_names[files.device()], filename? filename: "No file");
}

static void reset(bool sd) {

	input.reset();
	disk.reset();

	switches.on_read_keyboard([]() { return input.read(); });
	switches.on_strobe_keyboard([]() { input.strobe(); });

	switches.on_access_page(screen_page_change);
	switches.on_access_graphics_text(screen_mode_change);
	switches.on_access_full_mixed(screen_mode_change);
	switches.on_access_res(screen_mode_change);

	switches.on_access_speaker([]() { digitalWrite(PWM_SOUND, !digitalRead(PWM_SOUND)); });

	machine.set_cpu_debugging(debug_never);

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
	}
	file_status();
}

void setup() {

	machine.begin();

	display.begin(BG_COLOUR, FG_COLOUR, ORIENT);
	display.setScreen(CHAR_WIDTH * CHARS_PER_LINE, CHAR_HEIGHT * SCREEN_LINES);
	display.clear();

	for (unsigned i = 0; i < 8; i++)
		memory.put(sys[i], i * ram<>::page_size);
	memory.put(hgr_page1, 0x2000);
	memory.put(hgr_page2, 0x4000);
	memory.put(user, 0x6000);

	systemio.put(switches, 0x00);
	systemio.put(disk_switches, soft_switches_offset(DISKII_SLOT));
	memory.put(systemio, 0xc000);
	memory.put(disk.bootprom, 0xc600);
	memory.put(language, langaddr);

#if defined(LANGUAGE_CARD)
	systemio.put(lang_switches, soft_switches_offset(LANGCARD_SLOT));
#else
	memory.put(monitor, 0xf800);
#endif

	kbd.register_fnkey_handler(function_key);
	machine.interval_timer(FLASH_INTERVAL, flash_text);
	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {

	machine.run(1023000);
}
