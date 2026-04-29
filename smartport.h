#pragma once

class SmartPort {
public:
	SmartPort(Memory &memory, flash_file &hd1, flash_file &hd2);

	prom bootprom;

	class Switches: public Memory::Device {
	public:
		Switches(r6502 &cpu, SmartPort &sp): Memory::Device(16), _cpu(cpu), _sp(sp) {}

		operator uint8_t() override;
		void operator=(uint8_t) override {}

	private:
		uint8_t _block_driver_wrapper();
		uint8_t _smartport_wrapper();

		r6502 &_cpu;
		SmartPort &_sp;
	};

private:
	uint8_t boot();
	uint8_t block_driver(uint8_t cmd, uint8_t unit, Memory::address ptr, uint32_t block);

	uint8_t smartport_driver(uint8_t cmd, Memory::address params);
	uint8_t cmd_status(flash_file &drive, Memory::address params);
	uint8_t cmd_read_block(flash_file &drive, Memory::address params);
	uint8_t cmd_write_block(flash_file &drive, Memory::address params);

	// low-level interface
	uint16_t read_block(flash_file &drive, uint32_t block, Memory::address dest);
	uint16_t write_block(flash_file &drive, uint32_t block, Memory::address src);

	Memory &_memory;
	flash_file &_hd1, &_hd2;
};
