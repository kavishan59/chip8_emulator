#include "chip8.h"
#include <time.h>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Incorrect args\n");
    return(1);
  }
  emulator emulator = {0};
  int status = -1;
  srand(time(NULL));
  if (!initialize_emulator(&emulator))
  {
    if (load_rom(&emulator.cpu, argv[1]) == 0)
    {
      status = 0;
      emulate(&emulator);
    }
    destroy_emulator(&emulator);
  }
  return status;
}
