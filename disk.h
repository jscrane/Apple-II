#pragma once

class DiskII {
public:
	DiskII(Memory &memory, flash_file &driveA, flash_file &driveB);
	void reset() { _boot = 0; }

	prom bootprom;

	class Switches: public Memory::Device {
	public:
		Switches(CPU &cpu, DiskII &disk): Memory::Device(16), _cpu(cpu), _disk(disk) {}

		operator uint8_t() override;
		void operator=(uint8_t) override {}

	private:
		CPU &_cpu;
		DiskII &_disk;
	};

private:
	uint8_t boot1();
	uint8_t boot2(Memory::address rwts);

	void seek(flash_file *drive, uint8_t trk, uint8_t sec);
	uint16_t read(flash_file *drive, Memory::address to, uint16_t bytes);
	uint16_t write(flash_file *drive, Memory::address from, uint16_t bytes);

	Memory &_memory;
	flash_file *_drives[2];
	uint8_t _vols[2];
	uint8_t _boot;
};
