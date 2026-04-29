#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pgmspace.h>

#include <machine.h>
#include <memory.h>
#include <CPU.h>
#include <r6502.h>
#include <prom.h>
#include <debugging.h>
#include <serialio.h>
#include <filer.h>
#include <flash_filer.h>

#include "config.h"
#include "smartport.h"

// https://github.com/Bad-Mango-Solutions/back-pocket-basic/blob/main/specs/os/SmartPort%20Specification.md
#define ROM_PAGE	(0xc0 + SMARTPORT_SLOT)
#define SLOT_OFFSET	(0x10 * SMARTPORT_SLOT)
#define SSWITCH(n)	(0x80 + SLOT_OFFSET + n)
#define BLOCK_SIZE	512

#define CMD_STATUS	0
#define CMD_READ_BLOCK	1
#define CMD_WRITE_BLOCK	2

#define NO_ERROR	0x00
#define BAD_COMMAND	0x01
#define BAD_UNIT	0x21
#define IO_ERROR	0x27
#define NO_DEVICE	0x28
#define WRITE_PROT	0x2b
#define OFFLINE		0x2f

#define RETURN_X	0xeb
#define RETURN_Y	0xec

static const uint8_t diskboot[] PROGMEM = {

	// signature
	0x4c, 0x20, ROM_PAGE,	//	JMP $Cn20 ($cn01 is $20 for ID #1)
	0x00,			//	ID #2
	0xff,			//	junk
	0x03,			//	ID #3
	0x00,			//	junk
	0x00,			//	ID #4 (should be 0x00 for smartport)

	// padding
	0x00, 0x00, 0x00, 0x00, 0x00,

	// $Cn0D: block driver entry point
	0x4c, 0x30, ROM_PAGE,	//	JMP $Cn30

	// $Cn10: smartport entry point (block driver +3)
	0xad, SSWITCH(2), 0xc0,	//	LDA $C0F2	(softswitch #2)
	0xf0, 0x03,		//	BEQ OK
	0x38,			//	SEC
	0x60,			//	RTS
	0x18,			// OK:	CLC
	0x60,			//	RTS

	// padding
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// $Cn20: boot entry point
	0xad, SSWITCH(0), 0xc0,	//	LDA $C0F0	(softswitch #0)
	0xf0, 0x03,		//	BEQ OK
	0x4c, 0x00, 0xc6,	//	JMP $C600
				// OK:
	0xa2, SLOT_OFFSET,	//	LDX #$70
	0x4c, 0x01, 0x08,	// 	JMP $0801

	// padding
	0x00, 0x00, 0x00,

	// $Cn30: block driver
	0xad, SSWITCH(1), 0xc0,	//	LDA $C0F1	(softswitch #1)
	0xf0, 0x02,		//	BEQ OK
	0x38,			//	SEC
	0x60,			//	RTS
	0x18,			// OK:	CLC
	0xa6, RETURN_X,		//	LDX $EB
	0xa2, RETURN_Y,		//	LDY $EC
	0x60,			//	RTS

	0x00, 0x00, 0x00,					// $c738
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c740
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c748
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c750
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c758
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c760
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c768
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c770
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c778
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c780
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c788
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c790
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c798
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7A0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7A8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7B0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7B8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7C0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7C8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7D0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7D8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7E0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7E8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		// $c7F0
	0x00, 0x00, 0x00, 0x00, 				// $c7F8
	0x00, 0x00,	// total blocks (0x0000 means: "ask via status")
	0x83,		// status byte (bits: 0=writeable, 1=readable, 7=removable)
	0x0d,		// entry-point low byte ($c70d)
};

SmartPort::SmartPort(Memory &memory, flash_file &hd1, flash_file &hd2):
	bootprom(diskboot, sizeof(diskboot)), _memory(memory), _hd1(hd1), _hd2(hd2)
{
}

uint8_t SmartPort::boot() {

	DBG_DISK("smartport: boot");

	if (!_hd1) {
		DBG_DISK("smartport: no boot device");
		return 0x01;
	}

	if (NO_ERROR != read_block(_hd1, 0, 0x0800)) {
		DBG_DISK("smartport: boot failed");
		return 0x01;
	}

	return 0x00;
}

uint8_t SmartPort::read_block(flash_file &drive, uint32_t block, Memory::address dest) {

	drive.seek(block * BLOCK_SIZE);

	uint16_t i;

	for (i = 0; i < BLOCK_SIZE && drive.more(); i++)
		_memory[dest++] = drive.read();

	if (i != BLOCK_SIZE) {
		DBG_DISK("smartport: failed to read full block: %d", block);
		return IO_ERROR;
	}
	return NO_ERROR;
}

uint8_t SmartPort::write_block(flash_file &drive, uint32_t block, Memory::address src) {

	drive.seek(block * BLOCK_SIZE);

	for (uint16_t i = 0; i < BLOCK_SIZE; i++)
		drive.write(_memory[src++]);

	return NO_ERROR;
}

uint8_t SmartPort::block_driver(uint8_t cmd, uint8_t unit, Memory::address ptr, uint32_t block) {

	flash_file &drive = (unit & 0x80)? _hd2: _hd1;
	if (!drive) {
		DBG_DISK("smartport: no file: %02x", unit);
		return OFFLINE;
	}

	switch (cmd) {
	case CMD_READ_BLOCK:
		return read_block(drive, block, ptr);

	case CMD_WRITE_BLOCK:
		return write_block(drive, block, ptr);

	case CMD_STATUS:
		uint32_t blocks = drive.size() / BLOCK_SIZE;
		_memory[RETURN_X] = blocks & 0xff;
		_memory[RETURN_Y] = (blocks >> 8) & 0xff;
		return NO_ERROR;
	}

	DBG_DISK("smartport: block_driver: unknown command: %d", cmd);
	return BAD_COMMAND;
}

uint8_t SmartPort::smartport_driver(uint8_t cmd, Memory::address params) {

	uint8_t unit = _memory[params + 1];
	flash_file &drive = (unit & 0x80)? _hd2: _hd1;
	if (!drive) {
		DBG_DISK("smartport: no file: %02x", unit);
		return OFFLINE;
	}

	switch (cmd) {
	case CMD_READ_BLOCK:
		return read_block(drive, read_block(params + 4), read_ptr(params + 2));

	case CMD_WRITE_BLOCK:
		return write_block(drive, read_block(params + 4), read_ptr(params + 2));

	case CMD_STATUS:
		return cmd_status(drive, params);
	}

	DBG_DISK("smartport: smartport_driver: unknown command: %d", cmd);
	return BAD_COMMAND;
}

uint8_t SmartPort::cmd_status(flash_file &drive, Memory::address params) {

	uint8_t code = _memory[params + 4];

	if (code == 0x00 || code == 0x03) {
		uint32_t blocks = drive.size() / BLOCK_SIZE;
		Memory::address status = read_ptr(params + 2);
		_memory[status] = 0xf0;		// Block device | Write allowed | Read allowed | Device online
		_memory[status + 1] = blocks & 0xff;
		_memory[status + 2] = (blocks >> 8) & 0xff;
		_memory[status + 3] = (blocks >> 16) & 0xff;

		if (code == 0x03) {		// DIB
			const char *name = "EMULATOR";
			int n = strlen(name);
			_memory[status + 4] = n;
			for (int i = 0; i < n; i++)
				_memory[status + 5 + i] = name[i];

			_memory[status + 21] = 0x02;	// device type and subtype: Generic hard disk
			_memory[status + 22] = 0x01;
			_memory[status + 23] = 0x01;	// firmware version: major
			_memory[status + 24] = 0x00;	// firmware version: minor
		}
		return NO_ERROR;
	}

	DBG_DISK("smartport: smartport_driver: unknown status command: %d", code);
	return BAD_COMMAND;
}

SmartPort::Switches::operator uint8_t() {

	DBG_DISK("smartport: switch: %x", _acc);
	switch (_acc & 0x0f) {
	case 0x00:
		return _sp.boot();
	case 0x01:
		return _block_driver_wrapper();
	case 0x02:
		return _smartport_wrapper();
	};

	DBG_DISK("smartport: unknown switch: %x", _acc);
	return 0x01;	// error
}

uint8_t SmartPort::Switches::_block_driver_wrapper() {

	Memory &mem = _cpu.memory();
	mem[RETURN_X] = _cpu.x();
	mem[RETURN_Y] = _cpu.y();

	uint8_t cmd = mem[0x42];
	uint8_t unit = mem[0x43];
	Memory::address ptr = mem[0x44] | (mem[0x45] << 8);
	uint16_t block = mem[0x46] | (mem[0x47] << 8);

	return _sp.block_driver(cmd, unit, ptr, block);
}

uint8_t SmartPort::Switches::_smartport_wrapper() {

	uint8_t sp = _cpu.s();
	Memory &mem = _cpu.memory();
	Memory::address ret = mem[0x101 + sp] | (mem[0x102 + sp] << 8);

	uint8_t cmd = mem[ret + 1];
	Memory::address params = mem[ret + 2] | (mem[ret + 3] << 8);

	ret += 3;
	mem[0x101 + sp] = ret & 0xff;
	mem[0x102 + sp] = (ret >> 8) & 0xff;

	return _sp.smartport_driver(cmd, params);
}
