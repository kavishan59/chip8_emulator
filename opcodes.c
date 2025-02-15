#include "chip8.h"
#include "graphic.h"
#include <stddef.h>


//Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels 
void opcode_DXYN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  cpu *cpu = &emulator->cpu;
  cpu->V[0xF] = 0;
  if (cpu->I + b1 >= MEMORY_SIZE)
    return;

  for(size_t i = 0; i < b1; i++)
  {
    Uint8 sprite_byte = cpu->memory[cpu->I + i]; //sprite row of 8 bits
    Uint8 y = (cpu->V[b2] + i) % PIXEL_BY_HEIGHT;  //wrap Y coordinates (if out of bond , reappear a the opposite side!)
    for(size_t j =0; j < 8; j++)
    {
      Uint8 x = (cpu->V[b3] + j) % PIXEL_BY_WIDTH; //same as Y coordinate

      if ((sprite_byte << j) & 0b10000000) // get the MSB if j = 0 and so on
      { 
        if(screen->pixels[x][y] == WHITE)
          cpu->V[0xF] = 1;  //collision detected !
        screen->pixels[x][y] = !screen->pixels[x][y]; //toggle the pixel
      }
    }
  }

}

//interpret opcodes
void interpret(emulator *emulator)
{
  Uint16 opcode = get_opcode(&emulator->cpu);
  Uint8 b3,b2,b1; //represent 4 bits each of opcode = Nb3b2b1
  b3 = (opcode & (0x0F00)) >> 8;
  b2 = (opcode & (0x00F0)) >> 4;
  b1 = (opcode & (0x000F));
  Uint8 action = get_action(&emulator->cpu.table, opcode);

  switch(action)
  {
    case 0:
      printf("0NNN ");
      break;
    case 23:
      printf("DXYN ");
      opcode_DXYN(emulator, &emulator->screen, b1, b2, b3);
      break;
    default:
      printf("Unknown OPCODE ");

  } 
  emulator->cpu.pc += 2;
}
