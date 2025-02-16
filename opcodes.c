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

  cpu->pc = ((b3 << 8) + (b2 << 4) + b1) - 2;
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
  emulator->cpu.V[0xF] = 0; // vF reset quirks
}

//Set Vx = Vx AND Vy.
void opcode_8XY2(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b3] & emulator->cpu.V[b2];
  emulator->cpu.V[0xF] = 0; // vF reset quirks
}

//Set Vx = Vx XOR Vy.
void opcode_8XY3(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b3] ^ emulator->cpu.V[b2];
  emulator->cpu.V[0xF] = 0; // vF reset quirks
}

//Set Vx = Vx + Vy, set VF = carry.
void opcode_8XY4(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  int tb3,tb2;
  tb3 = emulator->cpu.V[b3];
  tb2 = emulator->cpu.V[b2];

  emulator->cpu.V[b3] += emulator->cpu.V[b2];
  if (tb2 > 0xFF - tb3)
    emulator->cpu.V[0xF] = 1;
  else
    emulator->cpu.V[0xF] = 0;
}

//Set Vx = Vx - Vy, set VF = NOT borrow.
void opcode_8XY5(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  int tb3,tb2;
  tb3 = emulator->cpu.V[b3];
  tb2 = emulator->cpu.V[b2];
  emulator->cpu.V[b3] = emulator->cpu.V[b3] - emulator->cpu.V[b2]; 
  emulator->cpu.V[0xF] = tb2 <= tb3;
}

//Set Vx = Vx SHR 1. if lsb of Vx equal to 1 then VF is set to 1;
void opcode_8XY6(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if (emulator->shift_mode == 1)
    emulator->cpu.V[b3] = emulator->cpu.V[b2];//shifting quirks
  int tb3 = emulator->cpu.V[b3];
  emulator->cpu.V[b3] = emulator->cpu.V[b3] >> 1;
  if ((tb3 & 0b00000001) == 0b00000001)
    emulator->cpu.V[0xF] = 1;
  else
    emulator->cpu.V[0xF] = 0;
}

//Set Vx = Vy - Vx, set VF = NOT borrow.
void opcode_8XY7(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.V[b2] - emulator->cpu.V[b3]; 
  emulator->cpu.V[0xF] = emulator->cpu.V[b3] <= emulator->cpu.V[b2];
}

//Set Vx = Vx SHL 1. if msb of Vx equal to 1 then VF is set to 1;
void opcode_8XYE(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if (emulator->shift_mode == 1)
    emulator->cpu.V[b3] = emulator->cpu.V[b2];//shifting quirks
  int tb3 = emulator->cpu.V[b3];
  emulator->cpu.V[b3] = emulator->cpu.V[b3] << 1;
  if ((tb3 & 0b10000000) == 0b10000000)
    emulator->cpu.V[0xF] = 1;
  else
    emulator->cpu.V[0xF] = 0;
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
  Uint8 Y = (cpu->V[b2]) % PIXEL_BY_HEIGHT;
  Uint8 X = (cpu->V[b3]) % PIXEL_BY_WIDTH;
  emulator->cpu.V[0xF] = 0;
  
  for(size_t i = 0; i < b1; i++)
  {
    Uint8 sprite_byte = cpu->memory[cpu->I + i]; //sprite row of 8 bits
    Uint8 y = Y + i;
    
    if (y >= PIXEL_BY_HEIGHT)
      continue;

    for(size_t j =0; j < 8; j++)
    {
      Uint8 x = X + j;
      
      if(x >= PIXEL_BY_WIDTH)
        continue;

      if ((sprite_byte << j) & 0b10000000) // get the MSB if j = 0 and so on
      { 
        if(screen->pixels[x][y] == WHITE)
          cpu->V[0xF] = 1;//collision detected !
        screen->pixels[x][y] = !screen->pixels[x][y]; //toggle the pixel
      }
    }
  }

}

//Skip next instruction if key with the value of Vx is pressed.
void opcode_EX9E(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if(emulator->cpu.key[emulator->cpu.V[b3]])
    emulator->cpu.pc += 2;
}

//Skip next instruction if key with the value of Vx is not pressed.
void opcode_EXA1(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  if(!emulator->cpu.key[emulator->cpu.V[b3]])
    emulator->cpu.pc += 2;
}

//Set Vx = delay timer value.
void opcode_FX07(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.V[b3] = emulator->cpu.sys_counter; 
}

//Wait for a key press, store the value of the key in Vx.
void opcode_FX0A(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
    static SDL_bool pressed_key[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
    for (int i = 0; i < 16; i++) 
    {
        if (emulator->cpu.key[i]) // If key is pressed
        {
            pressed_key[i] = SDL_TRUE;
        }
    }
    for(int i = 0; i < 16; i++)
    {
      if (pressed_key[i] == SDL_TRUE && !emulator->cpu.key[i])
      {
            emulator->cpu.V[b3] = i;// Store the key value in VX
            memset(pressed_key, 0,sizeof(pressed_key)); //reset the static array
            return;                  // Continue execution
      }
    }

    // If no key is pressed, repeat opcode (pause execution)
    emulator->cpu.pc -= 2;
}

//Set delay timer = Vx.
void opcode_FX15(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.sys_counter = emulator->cpu.V[b3]; 
}

//Set sound timer = Vx.
void opcode_FX18(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.sound_counter = emulator->cpu.V[b3]; 
}

//Set I = I + Vx.
void opcode_FX1E(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.I += emulator->cpu.V[b3]; 
}

//Set I = location of sprite(font) for digit Vx. we stored from memory 0 to 80. Each font take 5 bytes
void opcode_FX29(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  emulator->cpu.I = 5 * emulator->cpu.V[b3];
}

//Store BCD representation of Vx in memory locations I, I+1, and I+2.
void opcode_FX33(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  cpu *cpu = &emulator->cpu;
  Uint8 nb = cpu->V[b3];
  emulator->cpu.memory[cpu->I + 2] = nb % 10;
  emulator->cpu.memory[cpu->I + 1] = (nb/10) % 10;
  emulator->cpu.memory[cpu->I] = nb/100;
}

//Store registers V0 through Vx in memory starting at location I.
void opcode_FX55(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  for(int i = 0; i <= b3 ; i++)
  {
    emulator->cpu.memory[emulator->cpu.I + i] = emulator->cpu.V[i];
  }
  if (emulator->load_mode == 1)
    emulator->cpu.I += 1; //memory quirks
}

//Read registers V0 through Vx from memory starting at location I.
void opcode_FX65(emulator *emulator, screen *screen, Uint8 b1, Uint8 b2, Uint8 b3)
{
  for(int i = 0; i <= b3 ; i++)
  {
    emulator->cpu.V[i] =emulator->cpu.memory[emulator->cpu.I + i];  
  }
  if (emulator->load_mode == 1)
    emulator->cpu.I += 1; //memory quirks
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
      printf("0NNN(%X) ",opcode);
      break;
    case 1:
      printf("00E0(%X) ",opcode);
      opcode_00E0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 2:
      printf("00EE(%X) ",opcode);
      opcode_00EE(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 3:
      printf("1NNN(%X) ",opcode);
      opcode_1NNN(emulator,&emulator->screen, b1,b2,b3);
      break;
    case 4:
      printf("2NNN(%X) ",opcode);
      opcode_2NNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 5:
      printf("3XNN(%X) ",opcode);
      opcode_3XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 6:
      printf("4XNN(%X) ",opcode);
      opcode_4XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 7:
      printf("5XY0(%X) ",opcode);
      opcode_5XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 8:
      printf("6XNN(%X) ",opcode);
      opcode_6XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 9:
      printf("7XNN(%X) ",opcode);
      opcode_7XNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 10:
      printf("8XY0(%X) ",opcode);
      opcode_8XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 11:
      printf("8XY1(%X) ",opcode);
      opcode_8XY1(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 12:
      printf("8XY2(%X) ",opcode);
      opcode_8XY2(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 13:
      printf("8XY3(%X) ",opcode);
      opcode_8XY3(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 14:
      printf("8XY4(%X) ",opcode);
      opcode_8XY4(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 15:
      printf("8XY5(%X) ",opcode);
      opcode_8XY5(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 16:
      printf("8XY6(%X) ",opcode);
      opcode_8XY6(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 17:
      printf("8XY7(%X) ",opcode);
      opcode_8XY7(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 18:
      printf("8XYE(%X) ",opcode);
      opcode_8XYE(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 19:
      printf("9XY0(%X) ",opcode);
      opcode_9XY0(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 20:
      printf("ANNN(%X) ",opcode);
      opcode_ANNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 21:
      printf("BNNN(%X) ",opcode);
      opcode_BNNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 22:
      printf("CXNN(%X) ",opcode);
      opcode_CXNN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 23:
      printf("DXYN(%X) ",opcode);
      opcode_DXYN(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 24:
      printf("EX9E(%X) ",opcode);
      opcode_EX9E(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 25:
      printf("EXA1(%X) ",opcode);
      opcode_EXA1(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 26:
      printf("FX07(%X) ",opcode);
      opcode_FX07(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 27:
      printf("FX0A(%X) ",opcode);
      opcode_FX0A(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 28:
      printf("FX15(%X) ",opcode);
      opcode_FX15(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 29:
      printf("FX18(%X) ",opcode);
      opcode_FX18(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 30:
      printf("FX1E(%X) ",opcode);
      opcode_FX1E(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 31:
      printf("FX29(%X) ",opcode);
      opcode_FX29(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 32:
      printf("FX33(%X) ",opcode);
      opcode_FX33(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 33:
      printf("FX55(%X) ",opcode);
      opcode_FX55(emulator, &emulator->screen, b1, b2, b3);
      break;
    case 34:
      printf("FX65(%X) ",opcode);
      opcode_FX65(emulator, &emulator->screen, b1, b2, b3);
      break;
    default:
      printf("Unknown OPCODE(%X) ",opcode);
      break;
  } 
  emulator->cpu.pc += 2;
}
