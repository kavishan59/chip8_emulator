#include "chip8.h"
#include "graphic.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <string.h>


//initialize our cpu , the program counter must start at adress 0x200
void initialize_cpu(cpu *cpu)
{
  memset(cpu, 0, sizeof(*cpu));
  cpu->pc = (Uint8)START_ADRESS;
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
    update_screen(&emulator->screen);
    SDL_Delay(16);
  }
}
