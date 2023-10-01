#include "Cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void resetTime();

unsigned int getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(int, unsigned char *, int);

/*********************** Cache *************************/

void initCache();
void accessL1(int, unsigned char *, int);

typedef struct CacheLine {
  unsigned char Valid;
  unsigned char Dirty;
  unsigned int Tag;
} CacheLine;

typedef struct Cache {
  int init;
  CacheLine lines[L1_SIZE / BLOCK_SIZE];
} Cache;

/*********************** Interfaces *************************/

void read(int, unsigned char *);

void write(int, unsigned char *);
