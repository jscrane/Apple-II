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

// see https://gswv.apple2.org.za/a2zine/GS.WorldView/Resources/DOS.3.3.ANATOMY/BOOT.PROCESS.txt
// https://6502disassembly.com/a2-rom/C600ROM.html

// the disk is accessed for the first time with PR #6
// "permanent redirect to slot #6"
// below we store A, Y, Y and S in a "screen hole"
// then trigger an illegal instruction which traps
// to Disk::on_illegal_instruction().
// on cpu.resume(), clear carry ("success") and return.
static const uint8_t diskboot[] PROGMEM = {

	// .org $c600
	0x02,			// illegal instruction
	0xa2, 0x60,		// ldx #$60
	0x4c, 0x01, 0x08,	// jmp $0801

	// 86 bytes padding
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// .org $c65c
	0x02,			// illegal instruction
	0x18,			// clc (= success)
	0xa2, 0x60,		// ldx #$60
	0x4c, 0x01, 0x08,	// jmp $0801
};

Disk::Disk(Memory &memory, flash_file &driveA, flash_file &driveB): bootprom(diskboot, sizeof(diskboot)), _memory(memory), _driveA(driveA), _driveB(driveB) {
}

void Disk::reset() {
	_drive = &_driveA;
	_boot = 0;
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

void Disk::on_illegal_instruction(Memory::address addr) {

	DBG_EMU("addr: %04x", addr);

	if (addr == 0xc600) {

		DBG_EMU("boot0");
		seek(0, 0);
		read(0x0800, BYTES_PER_SECTOR);
		_memory[0x27] = 0x08;		// data_ptr (hi)
		_memory[0x2b] = 0x60;		// slot number << 4
		_memory[0x3e] = 0x5c;		// jump vector
		_memory[0x3f] = 0xc6;
		_boot++;
		return;
	}

	if (addr == 0xc65c) {

		uint8_t bsectr = _memory[0x3d];
		Memory::address dest = (_memory[0x27] << 8);
		DBG_EMU("boot1: %02x %04x", bsectr, dest);
		seek(0, bsectr);
		read(dest, BYTES_PER_SECTOR);
		_memory[0x3e] = 0x5c;
		_memory[0x3f] = 0xc6;
		_boot++;
		return;
	}

}
