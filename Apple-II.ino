#include <r65emu.h>
#include <r6502.h>
#include <display.h>

#include "original_monitor.h"
#include "integer_basic1.h"
#include "integer_basic2.h"
#include "integer_basic3.h"
#include "config.h"
#include "textscreen.h"
#include "softswitches.h"

Memory memory;
r6502 cpu(memory);
Arduino machine(cpu);

prom monitor(original_monitor, sizeof(original_monitor));
prom basic1(integer_basic1, sizeof(integer_basic1));
prom basic2(integer_basic2, sizeof(integer_basic2));
prom basic3(integer_basic3, sizeof(integer_basic3));
ram<> lowram;
ram<> mainram[RAM_PAGES];

Display display;
TextScreen textscreen(display);
SoftSwitches switches;

static void reset(bool) {

	switches.on_access_text([](bool on) { textscreen.enable(on); });
#if defined(PWM_SOUND)
	switches.on_access_speaker([]() { digitalWrite(PWM_SOUND, !digitalRead(PWM_SOUND)); });
#endif

	textscreen.begin();
}

void setup() {

	machine.begin();

	memory.put(lowram, 0x0000);
	memory.put(textscreen, 0x0400);
	for (int i = 0; i < RAM_PAGES; i++)
		memory.put(mainram[i], 0x0800 + 0x0400*i);

	memory.put(switches, 0xc000);
	memory.put(basic1, 0xe000);
	memory.put(basic2, 0xe800);
	memory.put(basic3, 0xf000);
	memory.put(monitor, 0xf800);

	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {

	machine.run();
}
