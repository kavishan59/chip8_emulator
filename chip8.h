#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdio.h>
#include "graphic.h"

#define MEMORY_SIZE 4096
#define START_ADRESS 512
#define OPCODE_NB 35


//represent the opcode identifier using mask
struct s_jump
{
  Uint16 mask [OPCODE_NB];
  Uint16 id[OPCODE_NB];
};

//represent our cpu
typedef struct s_scpu
{
  Uint8   memory[MEMORY_SIZE];
  Uint8   V[16];
  Uint16  I;
  Uint16  jump[16]; //stack
  Uint8   jump_nb; //stack top
  Uint8   sys_counter;
  Uint8   sound_counter;
  Uint16   pc;
  struct s_jump table;
  SDL_bool key[16]; // chip8 keyboard
} cpu;

//represent our emulator
typedef struct s_emulator
{
  cpu cpu;
  screen screen;
  input input; //to manage sdl events
  int key_table[16]; // contains key_table[chip8_keyboard] = our_keyboard_value
  SDL_bool shift_mode;
  SDL_bool load_mode;
} emulator;


void initialize_cpu(cpu *cpu);
void count(cpu *cpu);
int load_rom(cpu *cpu, const char path[]);
int initialize_emulator (emulator *emulator);
void destroy_emulator(emulator *emulator);
void emulate(emulator *emulator);
Uint16 get_opcode(cpu *cpu);
Uint8 get_action(struct s_jump *table, Uint16 opcode);
void initiliaze_jump_table(struct s_jump *table);

void interpret(emulator *emulator);
void manage_input(emulator *emulator);

//OPCODES

#endif // !CHIP8_H
