#pragma once
#include <string>
class Chip_8
{
public:

	Chip_8();
	Chip_8(const Chip_8& other) = delete;
	Chip_8(Chip_8&& other) noexcept = delete;
	Chip_8& operator=(const Chip_8& other) = delete;
	Chip_8& operator=(Chip_8&& other) noexcept = delete;	


	void initialize();
	bool loadGame(char const* name);
	void emulateCycle();
	bool drawFlag;
	// hex based keypad
	unsigned char key[16];
	// pixels
	unsigned char gfx[64 * 32];
	
private:
	// chip-8 has 35 opcodes which are all two bytes long
// unsigned short is two bytes so we can use it.
	unsigned short opcode;

	// chip-8 has 4k memory in total so
	unsigned char memory[4096];

	// chip-8 has 15 8 bit general purpose registers names v0,v1,up to VE
	// the 16 the register is used to carry flag.
	unsigned char V[16];

	// index register I and program counter pc
	unsigned short I;
	unsigned short pc;

	unsigned short stack[16];
	unsigned short sp;
	// the graphics of chip-8 are black and whitte and the screen has a total of 2048
	// pixels
	//unsigned char gfx[64 * 32];

	unsigned char delay_timer;
	unsigned char sound_timer;

	

	
	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

};

