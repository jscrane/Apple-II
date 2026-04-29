#pragma once

//#define APPLE_II
//#define APPLE_II_PLUS
#define LANGUAGE_CARD

#define LANGCARD_SLOT	0
#define SMARTPORT_SLOT	5
#define DISKII_SLOT	6

#define BG_COLOUR	BLACK
#define FG_COLOUR	0x5667

//#define HIRES_MONO
#define HIRES_COLOUR

#if defined(HIRES_COLOUR) && defined(USE_TFT)
#define HIRES_REFRESH	1000000
#else
#define HIRES_REFRESH	40000
#endif

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

// 0=Tape, 1=D1, 2=D2
#define DEFAULT_DEVICE	1
#define MAX_DEVICES	3
