#include "chip8.h"
#include "graphic.h"
#include <SDL2/SDL_stdinc.h>
#include <stddef.h>

//Clear the display./
void opcode_00E0(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  clear_screen(screen);
}

//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer./
void opcode_00EE(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  cpu *cpu = &emulator->cpu;
  if (cpu->jump_nb > 0)
  {
    cpu->jump_nb--;
    cpu->pc = cpu->jump[cpu->jump_nb];
  }

}

//The interpreter sets the program counter to nnn.
void opcode_1NNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.pc = (b3 << 8) + (b2 << 4) + b1 - 2;
}

//The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
void opcode_2NNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  cpu *cpu =&emulator->cpu;
  cpu->jump[cpu->jump_nb] = cpu->pc;
  if (cpu->jump_nb < 15)
    cpu->jump_nb++;

  cpu->pc = (b3 << 8) + (b2 << 4) + b1 - 2;
}

//The interpreter compares register Vx to nn, and if they are equal, increments the program counter by 2.
void opcode_3XNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if (emulator->cpu.V[b3] == (b2 << 4) + b1)
    emulator->cpu.pc += 2; 
}

//Skip next instruction if Vx != NN. 
void opcode_4XNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{

  if (emulator->cpu.V[b3] != (b2 << 4) + b1)
    emulator->cpu.pc += 2; 

}

//Skip next instruction if Vx = Vy.
void opcode_5XY0(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if (emulator->cpu.V[b3] == emulator->cpu.V[b2])
    emulator->cpu.pc += 2;
}

//Set Vx = NN.
void opcode_6XNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = (b2 << 4) + b1;
}

//Set Vx = Vx + NN./
void opcode_7XNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] += (b2 << 4) + b1;
}

//Set Vx = Vy.
void opcode_8XY0(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b2];
}

//Set Vx = Vx OR Vy.
void opcode_8XY1(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b3] | emulator->cpu.V[b2];
}

//Set Vx = Vx AND Vy.
void opcode_8XY2(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b3] & emulator->cpu.V[b2];
}

//Set Vx = Vx XOR Vy.
void opcode_8XY3(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b3] ^ emulator->cpu.V[b2];
}

//Set Vx = Vx + Vy, set VF = carry.
void opcode_8XY4(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[0xF] = emulator->cpu.V[b2] > 0xFF - emulator->cpu.V[b3];
  emulator->cpu.V[b3] += emulator->cpu.V[b2];
}

//Set Vx = Vx - Vy, set VF = NOT borrow.
void opcode_8XY5(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[0xF] = emulator->cpu.V[b2] <= emulator->cpu.V[b3];
  emulator->cpu.V[b3] = emulator->cpu.V[b3] - emulator->cpu.V[b2]; 
}

//Set Vx = Vx SHR 1. if lsb of Vx equal to 1 then VF is set to 1;
void opcode_8XY6(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if ((b3 & 0b00000001) == 0b00000001)
    emulator->cpu.V[0xF] = 1;
  else
    emulator->cpu.V[0xF] = 0;
  emulator->cpu.V[b3] = emulator->cpu.V[b3] >> 1;
}

//Set Vx = Vy - Vx, set VF = NOT borrow.
void opcode_8XY7(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[0xF] = emulator->cpu.V[b3] <= emulator->cpu.V[b2];
  emulator->cpu.V[b3] = emulator->cpu.V[b2] - emulator->cpu.V[b3]; 
}

//Set Vx = Vx SHL 1. if msb of Vx equal to 1 then VF is set to 1;
void opcode_8XYE(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if ((b3 & 0b10000000) == 0b10000000)
    emulator->cpu.V[0xF] = 1;
  else
    emulator->cpu.V[0xF] = 0;
  emulator->cpu.V[b3] = emulator->cpu.V[b3] << 1;
}

//Skip next instruction if Vx != Vy.
void opcode_9XY0(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if (emulator->cpu.V[b3] != emulator->cpu.V[b2])
    emulator->cpu.pc += 2;
}

//Set I = nnn.
void opcode_ANNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.I = (b3 << 8) + (b2 << 4) + b1;
}

//Jump to location nnn + V0.
void opcode_BNNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.pc = (b3 << 8) + (b2 << 4) + b1 + emulator->cpu.V[0] - 2;
}

//Set Vx = random byte AND NN.
void opcode_CXNN(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  Uint8 random_value = rand() % 256;
  emulator->cpu.V[b3] = random_value & ((b2 << 4) + b1);
}

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

//Set I = location of sprite(font) for digit Vx. we stored from memory 0 to 80. Each font take 5 bytes
void opcode_FX29(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.I = 5 * emulator->cpu.V[b3];
}

//Store BCD representation of Vx in memory locations I, I+1, and I+2.
void opcode_FX33(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  Uint8 nb = emulator->cpu.V[b3];
  emulator->cpu.memory[emulator->cpu.I + 2] = nb % 10;
  emulator->cpu.memory[emulator->cpu.I + 1] = (nb/10) % 10;
  emulator->cpu.memory[emulator->cpu.I] = nb/100;
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
    case 1:
      printf("00E0 ");
      opcode_00E0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 2:
      printf("00EE ");
      opcode_00EE(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 3:
      printf("1NNN ");
      opcode_1NNN(emulator,&emulator->screen, b1,b2,b3);
      break;
    case 4:
      printf("2NNN ");
      opcode_2NNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 5:
      printf("3XNN ");
      opcode_3XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 6:
      printf("4XNN ");
      opcode_4XNN(emulator, &emulator->screen, b1, b2, b3);
    case 7:
      printf("5XY0 ");
      opcode_5XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 8:
      printf("6XNN ");
      opcode_6XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 9:
      printf("7XNN ");
      opcode_7XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 10:
      printf("8XY0 ");
      opcode_8XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 11:
      printf("8XY1 ");
      opcode_8XY1(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 12:
      printf("8XY2 ");
      opcode_8XY2(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 13:
      printf("8XY3 ");
      opcode_8XY3(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 14:
      printf("8XY4 ");
      opcode_8XY4(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 15:
      printf("8XY5 ");
      opcode_8XY5(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 16:
      printf("8XY6 ");
      opcode_8XY6(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 17:
      printf("8XY7 ");
      opcode_8XY7(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 18:
      printf("8XYE ");
      opcode_8XYE(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 19:
      printf("9XY0 ");
      opcode_9XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 20:
      printf("ANNN ");
      opcode_ANNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 21:
      printf("BNNN ");
      opcode_BNNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 22:
      printf("CXNN ");
      opcode_CXNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 23:
      printf("DXYN ");
      opcode_DXYN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 31:
      printf("FX29 ");
      opcode_FX29(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 32:
      printf("FX33 ");
      opcode_FX33(emulator, &emulator->screen, b1, b2, b3);
      break;
    default:
      printf("Unknown OPCODE ");
      break;
  } 
  emulator->cpu.pc += 2;
}
