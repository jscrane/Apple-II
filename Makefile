t ?= esp8266

TERMINAL_SPEED := 115200
TERMINAL_EXTRA_FLAGS := -C serialout.txt
CPPFLAGS = -Wall -O3 -DTERMINAL_SPEED=$(TERMINAL_SPEED)
CPPFLAGS += -DDEBUGGING=0x201
LIBRARIES = PS2KeyAdvanced PS2KeyMap Adafruit_GFX Adafruit_BusIO Wire SPI

ifeq ($t, esp8266)
BOARD := d1_mini
baud := 921600
eesz := 4M3M
xtal := 160
CPPFLAGS += -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=PIN_D8 -DTFT_DC=PIN_D1 \
	-DTFT_RST=-1 -DSPI_FREQUENCY=80000000 -DLOAD_GLCD \
	-DHARDWARE_H=\"hw/esp8bit.h\" -DUSE_HOST_KBD
LIBRARIES += TFT_eSPI SpiRAM LittleFS
endif

ifeq ($t, esp32)
LIBRARIES += FS Network

ifeq ($b, lilygo)
BOARD := ttgo-t7-v14-mini32
SERIAL_PORT := /dev/ttyACM0
CPPFLAGS += -DUSE_SD -DPS2_SERIAL_KBD=\"UK\"
LIBRARIES += ESP32Lib SD
endif
endif

ifeq ($t, rp2040)
BOARD := adafruit_feather_dvi
flash := 8388608_2097152
CPPFLAGS += -DUSE_HOST_KBD
LIBRARIES += LittleFS PicoDVI
endif

-include $t.mk
