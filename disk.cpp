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
#define CMD_FORMAT	4

#define NO_ERROR	0x00
#define WRITE_PROTECT	0x10
#define BAD_VOLUME	0x20
#define DRIVE_ERROR	0x40
#define READ_ERROR	0x80

// see https://gswv.apple2.org.za/a2zine/GS.WorldView/Resources/DOS.3.3.ANATOMY/BOOT.PROCESS.txt
// https://htmlpreview.github.io/?https://github.com/Michaelangel007/apple2_dos33/blob/master/dos33.html

// the disk is accessed for the first time with PR #6 "permanent redirect to slot #6"
static const uint8_t diskboot[] PROGMEM = {

	// .org $c600
	// https://6502disassembly.com/a2-rom/C600ROM.html
	0xea, 0xea,		// don't autostart
	//0xa2, 0x20,		// ldx #$20
	0xa0, 0x00,		// ldy #$00
	0xa2, 0x03,		// ldx #$03
				// CreateDecTabLoop
	0x86, 0x3c,		// stx bits
	0x8a,			// txa
	0x0a,			// asl A
	0x24, 0x3c,		// bit bits
	0xf0, 0x10,		// beq :reject
	0x05, 0x3c,		// ora bits
	0x49, 0xff,		// eor #$ff
	0x29, 0x7e,		// and #$7e
				// :check_dub0
	0xb0, 0x08,		// bcs :reject
	0x4a,			// lsr A
	0xd0, 0xfb,		// bne :check_dub0
	0x98,			// tya
	0x9d, 0x56, 0x03,	// sta CONV_TAB,x
	0xc8,			// iny
				// :reject
	0xe8,			// inx
	0x10, 0xe5,		// bpl CreateDecTabLoop
	0x20, 0x58, 0xff,	// jsr MON_IORTS
	0xba,			// tsx
	0xbd, 0x00, 0x01,	// lda STACK,x
	0x0a,			// asl A
	0x0a,			// asl A
	0x0a,			// asl A
	0x0a,			// asl A
	0x85, 0x2b,		// sta slot_index
	0xaa,			// tax
	0xbd, 0x8e, 0xc0,	// lda IWM_Q7_OFF,x
	0xbd, 0x8c, 0xc0,	// lda IWM_Q6_OFF,x
	0xbd, 0x8a, 0xc0,	// lda IWM_SEL_DRIVE_1,x
	0xbd, 0x89, 0xc0,	// lda IWM_MOTOR_ON,x
	0xa0, 0x50,		// ldy #$50
				// :seek_loop
	0xbd, 0x80, 0xc0,	// lda IWM_PH0_OFF,x
	0x98,			// tya
	0x29, 0x03,		// and #$03
	0x0a,			// asl A
	0x05, 0x2b,		// ora slot_index
	0xaa,			// tax
	0xbd, 0x81, 0xc0,	// lda IWM_PH0_ON,x
	0xa9, 0x56,		// lda #$56
	0x20, 0xa8, 0xfc,	// jsr MON_WAIT
	0x88,			// dey
	0x10, 0xeb,		// bpl :seek_loop
	0x85, 0x26,		// sta data_ptr
	0x85, 0x3d,		// sta sector
	0x85, 0x41,		// sta track
	0xa9, 0x08,		// lda #>BOOT1
	0x85, 0x27,		// sta data_ptr+1

	// .org $c65c ReadSector
				// :another
	0x02,			// illegal instruction
	0xe6, 0x27,		// inc data_ptr+1
	0xe6, 0x3d,		// inc sector
	0xa5, 0x3d,		// lda sector
	0xcd, 0x00, 0x08,	// cmp BOOT1
	0xa6, 0x2b,		// ldx slot_index
	0x90, 0xf3,		// bcc :another
	0x4c, 0x01, 0x08,	// jmp BOOT1+1
};

Disk::Disk(Memory &memory, flash_file &driveA, flash_file &driveB): bootprom(diskboot, sizeof(diskboot)), _memory(memory), _driveA(driveA), _driveB(driveB) {
}

void Disk::reset() {
	_boot = 0;
}

void Disk::seek(flash_file *drive, uint8_t trk, uint8_t sec) {

	drive->seek(BYTES_PER_SECTOR * (sec + trk * SECTORS_PER_TRACK));
}

uint16_t Disk::read(flash_file *drive, Memory::address addr, uint16_t bytes) {

	uint16_t i;
	for (i = 0; i < bytes && drive->more(); i++)
		_memory[addr++] = drive->read();
	return i;
}

uint16_t Disk::write(flash_file *drive, Memory::address addr, uint16_t bytes) {

	uint16_t i;
	for (i = 0; i < bytes && drive->more(); i++)
		drive->write(_memory[addr++]);
	return i;
}

static const uint8_t reverse_sector_map[] = {
	0, 7, 14, 6, 13, 5, 12, 4,
	11, 3, 10, 2, 9, 1, 8, 15
};

void Disk::on_illegal_instruction(Memory::address addr) {

	if (addr == 0xc65c) {
		// ROM address (BOOT0)
		uint8_t sector = reverse_sector_map[_memory[0x3d]];
		uint8_t track = _memory[0x41];
		Memory::address dest = _memory[0x26] | (_memory[0x27] << 8);

		DBG_EMU("boot1: (%d) %02x %02x %04x", _boot, track, sector, dest);
		seek(&_driveA, track, sector);
		read(&_driveA, dest, BYTES_PER_SECTOR);
		_boot++;

		if (_boot == 11) {
			// now we've read the first sector (BOOT0) _and_
			// the first 10 sectors (BOOT1). from now on
			// DOS will call RWTS (at $3d00), so patch that.
			//
			// RWTS begins with this, so keep it:
			// 3D00:84 48          STY IOBPL       ;UPON ENTRY, A&Y POINT AT THE
			// 3D02:85 49          STA IOBPH       ;I/O CONTROL BLOCK (IOB)
			//
			_memory[0x3d04] = 0x02;		// illegal
			_memory[0x3d05] = 0x18;		// clc (= success)
			_memory[0x3d06] = 0x60;		// rts
		}
	} else if (addr == 0x3d04 || addr == 0xbd04) {
		// $3d00 is RWTS in BOOT2, $bd00 is same, after relocation
		Memory::address rwts = (addr & 0xff00);
		Memory::address iobp = _memory[0x48] | (_memory[0x49] << 8);
		uint8_t drive_id = _memory[iobp + 0x02];
		uint8_t track = _memory[iobp + 0x04];
		uint8_t sector = _memory[iobp + 0x05];
		uint8_t cmd = _memory[iobp + 0x0c];
		Memory::address buf = _memory[iobp + 8] | (_memory[iobp + 9] << 8);
		DBG_EMU("boot2: cmd=%d drive=%d %02x %02x %04x", cmd, drive_id, track, sector, buf);

		flash_file *drive = drive_id == 1? &_driveA: &_driveB;
		if (!*drive) {
			_memory[iobp + 0x0d] = READ_ERROR;
			_memory[rwts + 0x05] = 0x38;	// sec (= error)
			return;
		}

		seek(drive, track, sector);
		if (cmd == CMD_READ)
			read(drive, buf, BYTES_PER_SECTOR);
		else if (cmd == CMD_WRITE)
			write(drive, buf, BYTES_PER_SECTOR);
		// FIXME: format?

		_memory[iobp + 0x0d] = NO_ERROR;
		_memory[rwts + 0x05] = 0x18;	// clc (= success)
	} else
		DBG_EMU("sigill: unhandled %04x", addr);
}
