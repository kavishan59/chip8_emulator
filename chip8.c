#include "chip8.h"
#include "graphic.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

unsigned char chip8_fontset[80] = {
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


//initialize our cpu , the program counter must start at adress 0x200
void initialize_cpu(cpu *cpu)
{
  memset(cpu, 0, sizeof(*cpu));
  cpu->pc = (Uint8)START_ADRESS;
  for(int i = 0; i < 80 ; i++)
    cpu->memory[i] = chip8_fontset[i];
  initiliaze_jump_table(&cpu->table);
}


// sys and sound counter must decrease at 60 hz (every 16 milisecond)
void count(cpu *cpu)
{
  if (cpu->sys_counter > 0)
    cpu->sys_counter--;
  if (cpu->sound_counter > 0)
    cpu->sound_counter--;
}

// open file (rom) and stock in our cpu memory
int load_rom(cpu *cpu, const char path[])
{
  FILE *rom = fopen(path, "rb");
  if (!rom)
  {
    perror("Error fopen:");
    return -1;
  }
  fread(&cpu->memory[START_ADRESS], sizeof(Uint8)*(MEMORY_SIZE - START_ADRESS), 1, rom);
  fclose(rom);
  return 0;
}

int initialize_emulator (emulator *emulator)
{
  int status = -1;
  initialize_cpu(&emulator->cpu);
  memset(&emulator->input, 0, sizeof(input));
  if (initialize_SDL() == 0)
  {
    status = initialize_screen(&emulator->screen);
    if (status < 0)
      destroy_SDL();
  }
  return status;
}

void destroy_emulator(emulator *emulator)
{
  destroy_screen(&emulator->screen);
  destroy_SDL();
}

//our main loop 
void emulate(emulator *emulator)
{
    
  while(!emulator->input.quit)
  {
    update_event(&emulator->input);
    if(emulator->input.resize)
      resize_screen(&emulator->screen);
    

    update_screen(&emulator->screen);
    if (emulator->cpu.sys_counter > 0)
      emulator->cpu.sys_counter--;
    if (emulator->cpu.sound_counter > 0)
      emulator->cpu.sound_counter--;
    SDL_Delay(16);
  }
}

//fetch opcode from the memory
Uint16 get_opcode(cpu *cpu)
{
  return((Uint16)cpu->memory[cpu->pc] << 8 | (Uint16)cpu->memory[cpu->pc + 1]);
}

Uint8 get_action(struct s_jump *table, Uint16 opcode)
{
  for(size_t i = 0; i < OPCODE_NB; i++)
  {
    if((table->mask[i] & opcode) == table->id[i])
      return i;
  }
  fprintf(stderr, "Bad action, unknown opcode %d\n", opcode);
  return 0;
}

void initiliaze_jump_table(struct s_jump *table)
{
    table->mask[0]= 0x0000;  table->id[0]=0x0FFF;           /* 0NNN */
    table->mask[1]= 0xFFFF;  table->id[1]=0x00E0;           /* 00E0 */
    table->mask[2]= 0xFFFF;  table->id[2]=0x00EE;           /* 00EE */
    table->mask[3]= 0xF000;  table->id[3]=0x1000;           /* 1NNN */
    table->mask[4]= 0xF000;  table->id[4]=0x2000;           /* 2NNN */
    table->mask[5]= 0xF000;  table->id[5]=0x3000;           /* 3XNN */
    table->mask[6]= 0xF000;  table->id[6]=0x4000;           /* 4XNN */
    table->mask[7]= 0xF00F;  table->id[7]=0x5000;           /* 5XY0 */
    table->mask[8]= 0xF000;  table->id[8]=0x6000;           /* 6XNN */
    table->mask[9]= 0xF000;  table->id[9]=0x7000;           /* 7XNN */
    table->mask[10]= 0xF00F; table->id[10]=0x8000;          /* 8XY0 */
    table->mask[11]= 0xF00F; table->id[11]=0x8001;          /* 8XY1 */
    table->mask[12]= 0xF00F; table->id[12]=0x8002;          /* 8XY2 */
    table->mask[13]= 0xF00F; table->id[13]=0x8003;          /* BXY3 */
    table->mask[14]= 0xF00F; table->id[14]=0x8004;          /* 8XY4 */
    table->mask[15]= 0xF00F; table->id[15]=0x8005;          /* 8XY5 */
    table->mask[16]= 0xF00F; table->id[16]=0x8006;          /* 8XY6 */
    table->mask[17]= 0xF00F; table->id[17]=0x8007;          /* 8XY7 */
    table->mask[18]= 0xF00F; table->id[18]=0x800E;          /* 8XYE */
    table->mask[19]= 0xF00F; table->id[19]=0x9000;          /* 9XY0 */
    table->mask[20]= 0xF000; table->id[20]=0xA000;          /* ANNN */
    table->mask[21]= 0xF000; table->id[21]=0xB000;          /* BNNN */
    table->mask[22]= 0xF000; table->id[22]=0xC000;          /* CXNN */
    table->mask[23]= 0xF000; table->id[23]=0xD000;          /* DXYN */
    table->mask[24]= 0xF0FF; table->id[24]=0xE09E;          /* EX9E */
    table->mask[25]= 0xF0FF; table->id[25]=0xE0A1;          /* EXA1 */
    table->mask[26]= 0xF0FF; table->id[26]=0xF007;          /* FX07 */
    table->mask[27]= 0xF0FF; table->id[27]=0xF00A;          /* FX0A */
    table->mask[28]= 0xF0FF; table->id[28]=0xF015;          /* FX15 */
    table->mask[29]= 0xF0FF; table->id[29]=0xF018;          /* FX18 */
    table->mask[30]= 0xF0FF; table->id[30]=0xF01E;          /* FX1E */
    table->mask[31]= 0xF0FF; table->id[31]=0xF029;          /* FX29 */
    table->mask[32]= 0xF0FF; table->id[32]=0xF033;          /* FX33 */
    table->mask[33]= 0xF0FF; table->id[33]=0xF055;          /* FX55 */
    table->mask[34]= 0xF0FF; table->id[34]=0xF065;          /* FX65 */
}
