#include <unistd.h>

#include <machine.h>
#include <memory.h>
#include <prom.h>
#include <debugging.h>
#include <serialio.h>
#include <filer.h>
#include <flash_filer.h>

#include "disk.h"

// geometry
#define BYTES_PER_SECTOR	256
#define SECTORS_PER_TRACK	16
#define MAX_TRACK	35
#define CMD_READ	1
#define CMD_WRITE	2

// the disk is accessed for the first time with PR #6
// "permanent redirect to slot #6"
// below we store A, Y, Y and S in a "screen hole"
// then trigger an illegal instruction which traps
// to Disk::on_illegal_instruction().
// on cpu.resume(), clear carry ("success") and return.
static const uint8_t diskboot[] PROGMEM = {

	// .org $c600
	0x8d, 0xf8, 0x04,	// sta $04f8
	0x8e, 0xf9, 0x04,	// stx $04f9
	0x8c, 0xfa, 0x04,	// sty $04fa
	0xba,			// tsx
	0x8e, 0xfb, 0x04,	// stx $04fb
	0x02,			// illegal instruction
	0xae, 0xf9, 0x04,	// ldx $04f9
	0x18,			// clc
	0x60,			// rts
};

prom bootprom(diskboot, sizeof(diskboot));

Disk::Disk(Memory &memory, flash_file &driveA, flash_file &driveB): _memory(memory), _driveA(driveA), _driveB(driveB) {
	memory.put(bootprom, 0xc600);
	_drive = &driveA;	// FIXME
}

void Disk::seek(uint8_t trk, uint8_t sec) {

	_drive->seek(BYTES_PER_SECTOR * (sec + trk * SECTORS_PER_TRACK));
}

uint16_t Disk::read(Memory::address addr, uint16_t bytes) {

	uint16_t i;
	for (i = 0; i < bytes && _drive->more(); i++)
		_memory[addr++] = _drive->read();
	return i;
}

uint16_t Disk::write(Memory::address addr, uint16_t bytes) {

	uint16_t i;
	for (i = 0; i < bytes && _drive->more(); i++)
		_drive->write(_memory[addr++]);
	return i;
}

void Disk::on_illegal_instruction() {

	static bool dos_loaded = false;
	uint8_t A = _memory[0x04f8], X = _memory[0x04f9], Y = _memory[0x04fa], S = _memory[0x04fb];

	DBG_EMU("sigill: %02x %02x %02x %02x", A, X, Y, S);

	if (dos_loaded) {
		Memory::address iob = (A << 8) | Y;
		uint8_t cmd = _memory[iob+12], trk = _memory[iob+4], sec = _memory[iob+5];
		Memory::address addr = (_memory[iob+9] << 8) | _memory[iob+8];

		seek(trk, sec);
		if (cmd == CMD_READ)
			read(addr, BYTES_PER_SECTOR);
		else if (cmd == CMD_WRITE)
			write(addr, BYTES_PER_SECTOR);

	} else {
		// load (track 0, sectors 0-9) to $0800 (10 sectors, 2560 bytes)
		seek(0, 0);
		read(0x0800, 10 * BYTES_PER_SECTOR);

		// load (track 0, sector A to track 2, sector C) to $9d00 (35 sectors, 8960 bytes)
		read(0x9d00, 35 * BYTES_PER_SECTOR);

		// RWTS hook
		_memory[0xbd00] = 0x4c;
		_memory[0xbd01] = 0x00;
		_memory[0xbd02] = 0xc6;

		// tweak return address
		_memory[0x0100 + S + 1] = 0x83;
		_memory[0x0100 + S + 2] = 0x9d;

		// set current drive
		_memory[0x2b] = 0x60;
		_memory[0xb798] = 0x60;
 
		dos_loaded = true;
	}
}
