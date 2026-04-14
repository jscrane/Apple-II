#pragma once

//#define APPLE_II
#define APPLE_II_PLUS

#define DISK_SLOT	6

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

//#define HIRES_MONO
#define HIRES_COLOUR

#if defined(USE_ESPI)
#define ORIENT		reverse_landscape
#else
#define ORIENT		portrait
#endif

#if defined(USE_SD)
#define PROGRAMS	"/apple2/"
#else
#define PROGRAMS	"/"
#endif

#define RAM_TOP         0xc000u
#if (RAM_SIZE >= RAM_TOP)
#define RAM_PAGES	(RAM_TOP / ram<>::page_size)

#elif defined(USE_SPIRAM)
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)
#define SPIRAM_BASE	RAM_SIZE
#define SPIRAM_EXTENT	min(RAM_TOP - SPIRAM_BASE, SPIRAM_SIZE)

#else
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)

#endif
