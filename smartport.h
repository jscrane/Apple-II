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
		uint8_t _boot1_wrapper();

		r6502 &_cpu;
		SmartPort &_sp;
	};

private:
	uint8_t boot();
	uint8_t boot1(uint8_t op, uint8_t unit, Memory::address ptr, uint16_t block);
	uint8_t mli(uint8_t op, Memory::address params);

	uint16_t read_block(flash_file &drive, uint32_t block, Memory::address dest);
	uint16_t write_block(flash_file &drive, uint32_t block, Memory::address src);

	Memory &_memory;
	flash_file &_hd1, &_hd2;
};
