#include "L1Cache.h"

int main() {

  int value0, value1, value2, clock;

  resetTime();
  initCache();
  value0 = 0xAAAAAAAA;
  value1 = -1;
  value2 = 0;

  write(1, (unsigned char *)(&value1));
  clock = getTime();
  printf("Time: %d\n", clock);

  write(512, (unsigned char *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(1, (unsigned char *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(512, (unsigned char *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  return 0;
}