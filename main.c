#include "chip8.h"
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
  if (argc < 2 || argc > 4)
  {
    printf("Correct format: ./chip8 path_to_rom [-s] [-l]\n");
    return(1);
  }
  emulator emulator = {0};
  if ((argv[2] && strcmp(argv[2], "-s") == 0) || (argv[3] && strcmp(argv[3], "-s") == 0))
    emulator.shift_mode = 0;
  else
    emulator.shift_mode = 1;
  if ((argv[2] && strcmp(argv[2], "-l") == 0) || (argv[3] && strcmp(argv[3], "-l") == 0))
    emulator.load_mode = 1;
  else
    emulator.load_mode = 0;

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
