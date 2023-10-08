#include "L1Cache.h"

int main() {

  uint32_t value0, value1, value2, clock;

  resetTime();
  initCache();
  value0 = 0xAAAAAAAA;
  value1 = -1;
  value2 = 0;

  write(1, (uint8_t *)(&value1));
  clock = getTime();
  printf("Time: %d\n", clock);

  write(16384, (uint8_t *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(1, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(16384, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  return 0;
}
