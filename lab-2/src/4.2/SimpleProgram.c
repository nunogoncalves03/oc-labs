#include "L2Cache.h"

int main() {

  int value0, value1, value2, value3, clock;

  resetTime();
  initCache();
  value1 = 0x23232323;
  value2 = 0x23232323;
  value3 = 0x10101010;

  write(9, (unsigned char *)(&value1));
  clock = getTime();
  printf("Time: %d\n", clock);

  write(515, (unsigned char *)(&value2));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(11, (unsigned char *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  read(513, (unsigned char *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  read(516, (unsigned char *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Should be zero: %d\n", value0);

  write(514, (unsigned char *)(&value3));
  clock = getTime();
  printf("Time: %d\n", clock);

  read(513, (unsigned char *)(&value0));
  clock = getTime();
  printf("Time: %d\n", clock);
  printf("Value 0: %d\n", value0);

  return 0;
}