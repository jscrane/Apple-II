#pragma once

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

#define CHARS_PER_LINE	40
#define SCREEN_LINES	24
#define CHAR_WIDTH	7
#define CHAR_HEIGHT	8

#define ORIENT		reverse_landscape

#define RAM_TOP         0xc000u
#if (RAM_SIZE >= RAM_TOP)
#define RAM_PAGES	(RAM_TOP / ram<>::page_size)

#elif defined(USE_SPIRAM)
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)
#define SPIRAM_BASE	RAM_SIZE
#define SPIRAM_EXTENT	min(RAM_TOP - SPIRAM_BASE, SPIRAM_SIZE) / Memory::page_size

#else
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)

#endif
