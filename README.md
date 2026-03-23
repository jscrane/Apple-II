# Apple-II

Emulates an Apple-][ with 48kB of RAM.

Keyboard
--------
- F1 (^N): reset
- F2 (^O): advance to next file on current device
- F3 (^P): rewind current device
- F5 (^R): load program from tape (by simulating typing it)
- F8 (^U): select destination device (Tape, A:, B:)
- F10(^W): watch CPU execute instructions (if debugging enabled)

Tested Platforms
----------------

Please see [r65emu](https://github.com/jscrane/r65emu) for dependency information.

- ESP8266 with ILI9341 TFT display, serial terminal and LittleFS (`esp8bit`).
- [LilyGO TTGO](https://www.tinytronics.nl/shop/en/development-boards/microcontroller-boards/with-wi-fi/lilygo-ttgo-vga32-esp32) with VGA, PS/2 keyboard and SD card.
- [Adafruit Feather](https://www.adafruit.com/product/4884) with DVI, serial terminal and LittleFS.

Programs
--------

The `data` directory contains some simple programs from [Applesoft Sampler - 680-0179-A.dsk](https://mirrors.apple2.org.za/Apple%20II%20Diskware/DOS/).

Disk images (`.dsk`) are from [here](https://mirrors.apple2.org.za/Apple%20II%20Diskware/DOS/)

On an `esp8266`, the filesystem can be uploaded using:
```sh
$ make t=esp8266 littlefs upload-littlefs
```

On an `esp32`, spiffs is used:
```sh
$ make t=esp8266 spiffs upload-spiffs
```
