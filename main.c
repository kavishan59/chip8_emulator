#include "chip8.h"
#include <time.h>

int main(void)
{
  emulator emulator = {0};
  int status = -1;
  srand(time(NULL));
  if (!initialize_emulator(&emulator))
  {
    status = 0;
    emulate(&emulator);
    destroy_emulator(&emulator);
  }
  return status;
}
