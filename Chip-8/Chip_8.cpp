#include "pch.h"
#include "Chip_8.h"

#include <fstream>
#include <iostream>


#define _CRT_SECURE_NO_WARNINGS

Chip_8::Chip_8()
{
}

void Chip_8::initialize()
{
	//initalize register and memory once.
	pc = 0x200; // program counter start pos
	opcode = 0; // reset the current opcode
	I = 0; // reset index register
	sp = 0; // reset stack pointer


	// Clear display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;
	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;


	// load the fontset starting from 0x50 == 80
	for (int i = 0; i < 80; ++i)
	{
		memory[i] = chip8_fontset[i];
	}
	drawFlag = true;
	// system expects the application to be loaded at 0x200
	srand(time(NULL));
}

bool Chip_8::loadGame(char const* filename)
{
	

	initialize();
	printf("Loading: %s\n", filename);

	// Open file
	FILE* pFile;
	fopen_s(&pFile, filename, "rb");
	if (pFile == NULL)
	{
		fputs("File error", stderr);
		return false;
	}

	// Check file size
	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	char* buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if ((4096 - 512) > lSize)
	{
		for (int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;

}

void Chip_8::emulateCycle()
{
	// fetch opcode
		// :in fetch  the system will fetch one opcode from the memory at the location specified by
		// :program counter
		// :the opcode is two bytes long i.e two addressed so
		// :we fetch two successive byts and merge them to get the
		// :actual opcode
		// :the bytes are 8 bits so we displace the first by 8 bits
	opcode = memory[pc] << 8 | memory[pc + 1];
	//decode opcode

		// :stored opcode whould be decoded
		// :so we have to get what it means
		// :
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
			// this clears the screen
		case 0x0000:
			for (int i = 0; i < 2048; ++i)
			{
				gfx[i] = 0x0;
			}
			drawFlag = true;
			pc += 2;
			break;

			// returns from subroutine
		case 0x000E:
			// decrease stack to prevent override
			--sp;
			// put the stored return address from stack to program counter
			pc = stack[sp];
			// increase the program counter
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		// 0xNNN it jumps to address NNN
		break;
	case 0x1000:
		pc = opcode & 0x0FFF;
		break;
		// OX2NNN calls subroutine at NNN
	case 0x2000:
		// store current address in stack
		stack[sp] = pc;
		// increase stack pointer
		++sp;
		// set the program counter to address at NNN
		pc = opcode & 0x0FFF;
		break;



	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			// skips the next instruction if VX equals NN
			pc += 4;
		}
		else
		{
			// usually the next instruction is a jump to skip a code block
			pc += 2;
		}
		break;
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			// skips the next instruction if VX equals NN
			pc += 4;
		}
		else
		{
			// usually the next instruction is a jump to skip a code block
			pc += 2;
		}
		break;
	case 0x5000: //0x5XY0
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
		{
			// skips the next instruction if VX equals Vy
			pc += 4;
		}
		else
		{
			// usually the next instruction is a jump to skip a code block
			pc += 2;
		}
		break;

	case 0x6000: //0x6XNN
		// sets VX to NN
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;
	case 0x7000: //0x6XNN
	// sets VX to NN
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
			case 0x0000: // 0X8Y0
				// sets value of Vx to Vy
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0001: //0X8Y1
				// sets vx to vy or vx
				V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0002: //0X8Y2
				// Sets VX to VX and VY. (Bitwise AND operation);
				V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0003: //0X8Y3
				// Sets VX to VX and VY. (Bitwise XOR operation);
				V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0004: //0X8Y4
				// Adds VY to VX. VF is set to
				// 1 when there's a carry, and to 0 when there is not.
				if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				{
					// carry
					V[0xF] = 1;
				}
				else
				{
					V[0xF] = 0;
				}
				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;

			case 0x0005: //0x8Y5

				if (V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8]))
				{
					// carry
					//VF is set to 0 when
				// there's a borrow
					V[0xF] = 0;
				}
				else
				{
					//V[0xF]  = 1 when there is not.
					V[0xF] = 1;
				}
				// VY is subtracted from VX.
				V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;

			case 0x0006: //0x8Y6
				// Stores the least significant bit of VX in VF
				// and then shifts VX to the right by 1.
				V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
				V[(opcode & 0x0F00) >> 8] >>= 1;
				pc += 2;
				break;
			case 0x0007: //0x8Y7
				// Sets VX to VY minus VX. VF is set to 0 when
				// there's a borrow, and 1 when there is not.
				if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				{
					V[0xF] = 0;
				}
				else
				{
					V[0xF] = 1;
				}
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x000E:
				//Stores the most significant bit
				//of VX in VF and then shifts VX to the left by 1.[b]
				V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
				V[(opcode & 0x0F00) >> 8] <<= 1;
				pc += 2;
				break;
			}

		break;
	case 0x9000:


		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
		{
			// Skips the next instruction if VX does not equal VY.
			pc += 4;
		}
		else
		{
			// (Usually the next instruction is a jump to skip a code block);
			pc += 2;
		}

		break;

	case 0xA000:
		// Sets I to the address NNN.
		I = opcode & 0x0FFF;
		pc += 2;
		break;

		//jumps to address nnn plus V0
	case 0xB000:
		pc = opcode & 0x0FFF + V[0];

		break;
	case 0xC000:
		//Sets VX to the result of a bitwise and operation on
		//a random number (Typically: 0 to 255) and NN.
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		pc += 2;
		break;
	case 0xD000:
	{
		//Draws a sprite at coordinate (VX, VY)
		//that has a width of 8 pixels and a height of N pixels.
		//Each row of 8 pixels is read as bit-coded starting from
		//memory location I; I value does not change after the execution
		//of this instruction. As described above, VF is set to 1 if any
		//screen pixels are flipped from set to unset when the sprit
		//e is drawn, and to 0 if that does not happen

		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[x + xline + ((y + yline) * 64)] == 1)
					{
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}

			}
		}
		drawFlag = true;
		pc += 2;
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF)
		{
			case 0x009E:
				// Skips the next instruction if the key stored in VX is pressed.
				if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				{
					pc += 4;
				}
				else
				{
					// (Usually the next instruction is a jump to skip a code block);
					pc += 2;
				}
			break;

			case 0x00A1:
				// Skips the next instruction if the key stored in VX is not pressed.
				// (Usually the next instruction is a jump to skip a code block);
				// Skips the next instruction if the key stored in VX is pressed.
				if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				{
					pc += 4;
				}
				else
				{
					// (Usually the next instruction is a jump to skip a code block);
					pc += 2;
				}

			break;
		}

	case 0xF000:
			switch (opcode & 0x00FF)
			{
					//Sets VX to the value of the delay timer.
				case 0x0007:// FX07
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
				break;

				case 0x000A:
				{
					//A key press is awaited, and then stored in VX.
					//(Blocking Operation. All instruction halted until next key event);
					bool keyPress = false;

					for (int i = 0; i < 16; ++i)
					{
						if (key[i] != 0)
						{
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					if (!keyPress)
						return;

					pc += 2;
				}
				break;

				case 0x0015:
					//Sets the delay timer to VX.
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;

				case 0x0018:
					//Sets the sound timer to VX.
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x001E:
					//Adds VX to I. VF is not affected.[c]
					if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
					{
						// if there is range overflow vx=1
						V[0xF] = 1;
					}
					else
					{
						// else vx =0
						V[0xF] = 0;
					}
					I  += V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x0029:
					//Sets I to the location of the sprite for the character in VX.
					//Characters 0-F (in hexadecimal) are represented by a 4x5 font.
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
				break;

				case 0x0033:
					//Stores the binary-coded decimal representation of VX, with the
					//most significant of three digits at the address in I, the middle
					//digit at I plus 1, and the least significant digit at I plus 2.
					//(In other words, take the decimal representation of VX,
					//place the hundreds digit in memory at location in I,
					//the tens digit at location I+1, and the ones digit at location I+2.);
					// 
					// place the hundreds digit in memory at location in I,
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					//the tens digit at location I+1,
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					// and the ones digit at location I+2.)
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
				break;

				case 0x0055:
					//Stores from V0 to VX (including VX) in memory,
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					{
						memory[I + i] = V[i];
					}
					//starting at address I. The offset from I
					//is increased by 1 for each value written,
					//but I itself is left unmodified.[d]
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;

break;
				case 0x0065:
					//Fills from V0 to VX (including VX) with values from memory,
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					{
						V[i] = memory[I + i];
					}
					//starting at address I. The offset from I is increased by 1
					//for each value read, but I itself is left unmodified.[d]
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
					break;
			}

			break;
		// execute oopcode
			// :here we execute the opcode in the emulator
			// :here only 12 bit contains the value so we remove the first four bits
			// :using and operator
	}

	if (delay_timer > 0)
	{
		--delay_timer;
	}

	if (sound_timer > 0)
	{

		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;

	}

}

