#pragma once

//#define APPLE_II
//#define APPLE_II_PLUS
#define LANGUAGE_CARD

#define LANGCARD_SLOT	0
#define DISKII_SLOT	6
#define SMARTPORT_SLOT	7

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

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
