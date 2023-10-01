#include "L1Cache.h"

unsigned char L1Cache[L1_SIZE];
unsigned char L2Cache[L2_SIZE];
unsigned char DRAM[DRAM_SIZE];
unsigned int time;
Cache SimpleCache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

unsigned int getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(int address, unsigned char *data, int mode) {

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

/*********************** L1 cache *************************/

void initCache() { SimpleCache.init = 0; }

void accessL1(int address, unsigned char *data, int mode) {
  unsigned int Address = (unsigned int) address;

  printf("Address %d\n", Address);

  unsigned int offset, index, Tag, MemAddress; // 24 bits tag, 5 bits index, 3 bits offset, 32 total bits
  unsigned char TempBlock[BLOCK_SIZE];

  /* init cache */
  if (SimpleCache.init == 0) {
    SimpleCache.init = 1;

    for (int i = 0; i < L1_SIZE / BLOCK_SIZE; i++) {
      SimpleCache.lines[i].Valid = 0;
    }
  }

  offset = Address & 0b111; // apply a mask to remove the tag and index

  index = Address >> 3; // remove offset
  index = index & 0b11111; // apply a mask to remove the tag

  Tag = Address >> (5 + 3); // remove index and offset

  MemAddress = (Address >> 3) << 3; // address of the block in memory

  printf("Tag %d\n", Tag);
  printf("Offset %d\n", offset);
  printf("Index %d\n", index);
  printf("MemAddress %d\n", MemAddress);

  CacheLine *Line = &SimpleCache.lines[index];

  /* access Cache */

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      unsigned int OldMemAddress = ((Line->Tag << 5) + index) << 3;
      accessDRAM(OldMemAddress, &(L1Cache[index * BLOCK_SIZE]),
                 MODE_WRITE); // then write back old block
    }

    memcpy(&(L1Cache[index * BLOCK_SIZE]), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(L1Cache[index * BLOCK_SIZE + offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(L1Cache[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void read(int address, unsigned char *data) {
  printf("=== L1 BEFORE READ ===\n");
  for (int i = 0; i < L1_SIZE; i++) {
        printf("%d ", L1Cache[i]);

        if ((i + 1) % 8 == 0) {
          printf("\n");
        }
  }
  printf("\n");

  accessL1(address, data, MODE_READ);

  printf("=== L1 AFTER READ ===\n");
  for (int i = 0; i < L1_SIZE; i++) {
        printf("%d ", L1Cache[i]);

        if ((i + 1) % 8 == 0) {
          printf("\n");
        }
  }
  printf("\n");
}

void write(int address, unsigned char *data) {
  printf("=== L1 BEFORE WRITE ===\n");
  for (int i = 0; i < L1_SIZE; i++) {
        printf("%d ", L1Cache[i]);

        if ((i + 1) % 8 == 0) {
          printf("\n");
        }
  }
  printf("\n");

  accessL1(address, data, MODE_WRITE);

  printf("=== L1 AFTER WRITE ===\n");
  for (int i = 0; i < L1_SIZE; i++) {
        printf("%d ", L1Cache[i]);

        if ((i + 1) % 8 == 0) {
          printf("\n");
        }
  }
  printf("\n");
}
