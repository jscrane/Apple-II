# Apple-II

Emulates an Apple-][ with 48kB of RAM with [r65emu](https://github.com/jscrane/r65emu).

## Features and Limitations

- Display modes: Text, Low resolution graphics and High resolution graphics
- Language Card: Slot #0
- SmartPort: `.po` and `.hdv` files in Slot #5
- Disk-II: `.dsk` files in Slot #6

It does not:
- Emulate the tape interface, it's just too hard!
- Emulate Double Hires, see [issue](https://github.com/jscrane/Apple-II/issues/29)
- Support `.2mg` files, see [issue](https://github.com/jscrane/Apple-II/issues/44)

## Keyboard

- F1 (^N): reset
- F2 (^O): advance to next file on current device
- F3 (^P): rewind current device
- F4 (^Q): select input device (`Tape`, `D1:`, `D2:`)
- F5 (^R): load program from tape (by simulating typing it)
- F6 (^S): checkpoint machine to file
- F7 (^T): restore machine from selected checkpoint

## Tested Platforms

Please see [r65emu](https://github.com/jscrane/r65emu) for dependency information.

- ESP8266 with ILI9341 TFT display, serial terminal and LittleFS (`esp8bit`).
- [LilyGO TTGO](https://www.tinytronics.nl/shop/en/development-boards/microcontroller-boards/with-wi-fi/lilygo-ttgo-vga32-esp32) with VGA, PS/2 keyboard and SD card.
- [Adafruit Feather](https://www.adafruit.com/product/4884) with DVI, serial terminal and LittleFS.

## Programs

The `data` directory contains some simple programs from [Applesoft Sampler - 680-0179-A.dsk](https://mirrors.apple2.org.za/Apple%20II%20Diskware/DOS/).

Disk images (`.dsk`) are from [here](https://mirrors.apple2.org.za/Apple%20II%20Diskware/DOS/)

The filesystem can be uploaded using:
```sh
$ make t=[esp8266 esp32 | rp2040] littlefs upload-littlefs
```

### Filtering

Programs and disk-images in the uploaded filesystem (or SD card) are filtered by the device:

- `Tape` reports only `.bas`, `.txt` and `.img` files:
  - `.bas`, `.int` and `.txt` files can be loaded using `F4`;
  - `.img` (checkpoint) files can be loaded using `F7` (restore from checkpoint).
- `D1` and `D2` report only `.dsk`, `.po` and `.hdv` files:
  - selecting an image-file using `F4` and `F2` is like inserting a disk in a drive,
  - once "inserted", a disk image can be booted using `PR #6` (for `.dsk`) or `PR #7` (for `.po` or `.hdv`).
