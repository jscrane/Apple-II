#pragma once

class SmartPort {
public:
	SmartPort(Memory &memory, flash_file &hd);

	prom bootprom;

	class Switches: public Memory::Device {
	public:
		Switches(r6502 &cpu, SmartPort &sp): Memory::Device(16), _cpu(cpu), _sp(sp) {}

		operator uint8_t() override;
		void operator=(uint8_t) override {}

	private:
		r6502 &_cpu;
		SmartPort &_sp;
	};

private:
	uint8_t boot();
	uint8_t mli(uint8_t op);

	Memory &_memory;
	flash_file &_hd;
};
