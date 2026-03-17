#pragma once

class Disk {
public:
	Disk(Memory &memory, flash_file &driveA, flash_file &driveB);
	void reset();

	prom bootprom;
	void on_illegal_instruction(Memory::address);

private:
	void seek(uint8_t trk, uint8_t sec);
	uint16_t read(Memory::address to, uint16_t bytes);
	uint16_t write(Memory::address from, uint16_t bytes);

	Memory &_memory;
	flash_file &_driveA, &_driveB, *_drive;
	uint8_t _boot;
};
