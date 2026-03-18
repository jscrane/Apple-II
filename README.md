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
