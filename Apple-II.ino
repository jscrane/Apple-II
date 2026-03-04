#include <r65emu.h>
#include <r6502.h>
#include <display.h>

#include "original_monitor.h"
#include "integer_basic1.h"
#include "integer_basic2.h"
#include "integer_basic3.h"
#include "config.h"
#include "screen.h"
#include "text.h"
#include "lores.h"
#include "softswitches.h"
#include "keyboard.h"

Memory memory;
r6502 cpu(memory);
Arduino machine(cpu);

prom monitor(original_monitor, sizeof(original_monitor));
prom basic1(integer_basic1, sizeof(integer_basic1));
prom basic2(integer_basic2, sizeof(integer_basic2));
prom basic3(integer_basic3, sizeof(integer_basic3));
ram<> pages[RAM_PAGES];

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
SoftSwitches switches;
Keyboard keyboard(kbd);

static void reset(bool) {

	keyboard.reset();

	switches.on_read_keyboard([]() { return keyboard.read(); });
	switches.on_strobe_keyboard([]() { keyboard.strobe(); });

	switches.on_access_page([](bool is_page2) {
		if (is_page2) {
			memory.put(pages[1], 0x0400);
			text.display(pages[2]);
			lores.display(pages[2]);
		} else {
			text.display(pages[1]);
			lores.display(pages[1]);
			memory.put(pages[2], 0x0800);
		}
	});
	switches.on_access_graphics_text([](bool is_text) {
		Screen &screen = is_text? static_cast<Screen&>(text): static_cast<Screen&>(lores);
		memory.put(screen, switches.is_page2()? 0x0800: 0x0400);
		screen.redraw();
	});

	switches.on_access_speaker([]() { digitalWrite(PWM_SOUND, !digitalRead(PWM_SOUND)); });

	text.begin();
}

static void function_key(uint8_t fn) {
	switch (fn) {
	case 1:
		machine.reset();
		break;
	case 10:
		machine.debug_cpu();
		break;
	}
}

void setup() {

	machine.begin();

	for (int i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], 0x0400*i);

	memory.put(switches, 0xc000);
	memory.put(basic1, 0xe000);
	memory.put(basic2, 0xe800);
	memory.put(basic3, 0xf000);
	memory.put(monitor, 0xf800);

	kbd.register_fnkey_handler(function_key);
	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {

	machine.run();
}
