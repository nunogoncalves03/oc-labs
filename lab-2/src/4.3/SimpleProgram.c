#include "L2Cache.h"

int main() {

  uint32_t value0, value1, value2, clock;

  resetTime();
  initCache();
  value1 = 0x23232323;
  value2 = 0xAAAAAAAA;

  write(0, (uint8_t *)(&value1));
  clock = getTime();
  printf("Time: %d\n", clock);

  write(32768, (uint8_t *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(4, (uint8_t *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  read(32768, (uint8_t *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  read(4, (uint8_t *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  return 0;
}
