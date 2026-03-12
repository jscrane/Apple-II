# Apple-II

Emulates an Apple-][ with 48kB of RAM.

Keyboard
--------
- F1 (^N): reset
- F2 (^O): advance tape
- F3 (^P): rewind tape
- F5 (^R): load program from tape (by simulating typing it)
- F10(^W): watch CPU execute instructions (if debugging enabled)

Programs
--------

The `data` directory contains some simple programs. Sources include:
- [Applesoft Sampler - 680-0179-A.dsk](https://mirrors.apple2.org.za/Apple%20II%20Diskware/DOS/)

On an `esp8266`, it can be uploaded using:
```sh
$ make t=esp8266 littlefs upload-littlefs
```

On an `esp32`, spiffs is used:
```sh
$ make t=esp8266 spiffs upload-spiffs
```
