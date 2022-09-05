# CHIP-8
What exactly is an emulator? An emulator is a type of interpreter that reads the original instructions written in machine code and compiled for the target computer, interprets them, and then replaces the functionality of the target machine on the host machine.

Chip-8 was created by RCA engineer Joe Weisbecker in 1970 COSMAC VIP microcomputer. It was intended as a simpler way to make small programs and games for the computer.

There are certain components that need to be understood before understanding how chip-8 works. -> 16 8-Bit Registers: All the operations on a CPU are done using registers.
-> 4096 Bytes of Memory As register space is small, we need memory. This space is segmented into three sections:

: some reserved for the CHIP-8 interpreter;

some reserved for 16 built-in characters (0-F);

: some memory are instructions from ROM, which will be stored at 0x200, and after that, the remaining space will be free to use.

-> 16-bit Index Register A special register is used to store memory addresses for use in operations. Why 16? because it used 0xFFF, which is too big for an 8-bit register.

-> 16-bit Program Counter It keeps track of which instruction it is to execute next.

-> 16-Level stack It keeps track of the order of execution when it calls a function. It can hold 16 different program counter
-> 8-bit stack pointer It is used to keep track of where in memory the CPU is executing. It tells us where in the 16-levels of the stack the most recent value was placed.

->8-bit DelayTimer: Timer used for timing.If there is a value, it will decrement at a rate of 60Hz, else it will stay zero.

-> 8-bit Sound Timer -> Input Keys

-> Display Memory It has an additional memory buffer used for storing the graphics for display. which is 64 pixels wide and 32 pixels high. Each pixel is either on or off. So, only two colors can be represented

I have used SDL to display the graphics.

Resources:https://austinmorlan.com/posts/chip8_emulator/

Resources:https://en.wikipedia.org/wiki/CHIP-8

Resources:https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
