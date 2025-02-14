#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdio.h>
#include "graphic.h"

#define MEMORY_SIZE 4096
#define START_ADRESS 512


//represent our cpu
typedef struct s_scpu
{
  Uint8   memory[MEMORY_SIZE];
  Uint8   V[16];
  Uint16  I;
  Uint16  jump[16]; //stack
  Uint8   jump_nb; //stack counter
  Uint8   sys_counter;
  Uint8   sound_counter;
  Uint16   pc;
} cpu;

//represent our emulator
typedef struct s_emulator
{
  cpu cpu;
  screen screen;
  input input;
} emulator;

void initialize_cpu(cpu *cpu);
void count(cpu *cpu);
int load_rom(cpu *cpu, const char path[]);
int initialize_emulator (emulator *emulator);
void destroy_emulator(emulator *emulator);
void emulate(emulator *emulator);

#endif // !CHIP8_H
