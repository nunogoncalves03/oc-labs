#include "L2Cache.h"

unsigned char L1CacheData[L1_SIZE];
unsigned char L2CacheData[L2_SIZE];
unsigned char DRAM[DRAM_SIZE];
unsigned int time;
L1CacheStruct L1Cache;
L2CacheStruct L2Cache;

void initCache() {
  L1Cache.init = 0;
  L2Cache.init = 0;
}

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

unsigned int getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(int address, unsigned char *data, int mode) {

  if (address >= DRAM_SIZE - BLOCK_SIZE + 1)
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

void accessL1(int address, unsigned char *data, int mode) {
  unsigned int Address = (unsigned int) address;

  printf("Address %d\n", Address);

  unsigned int offset, index, Tag, MemAddress; // 24 bits tag, 5 bits index, 3 bits offset
  unsigned char TempBlock[BLOCK_SIZE];

  /* init cache */
  if (L1Cache.init == 0) {
    L1Cache.init = 1;

    for (int i = 0; i < L1_SIZE / BLOCK_SIZE; i++) {
      L1Cache.lines[i].Valid = 0;
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

  CacheLine *Line = &L1Cache.lines[index];

  /* access Cache */

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessL2(MemAddress, TempBlock, MODE_READ); // get new block from L2
    
    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      unsigned int OldMemAddress = ((Line->Tag << 5) + index) << 3;
      accessL2(OldMemAddress, &(L1CacheData[index * BLOCK_SIZE]),
                 MODE_WRITE); // then write back old block
    }

    memcpy(&(L1CacheData[index * BLOCK_SIZE]), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    offset -= offset % WORD_SIZE;  // align with the corresponding word
    memcpy(data, &(L1CacheData[index * BLOCK_SIZE + offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    offset -= offset % WORD_SIZE;  // align with the corresponding word
    memcpy(&(L1CacheData[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

/*********************** L2 cache *************************/

void accessL2(int address, unsigned char *data, int mode) {
  unsigned int Address = (unsigned int) address;

  printf("Address %d\n", Address);

  unsigned int index, Tag, MemAddress; // 24 bits tag, 5 bits index, 3 bits offset
  unsigned char TempBlock[BLOCK_SIZE];

  /* init cache */
  if (L2Cache.init == 0) {
    L2Cache.init = 1;

    for (int i = 0; i < L2_SIZE / BLOCK_SIZE; i++) {
      L2Cache.lines[i].Valid = 0;
    }
  }

  index = Address >> 3; // remove offset
  index = index & 0b111111111; // apply a mask to remove the tag

  Tag = Address >> (9 + 3); // remove index and offset

  MemAddress = (Address >> 3) << 3; // address of the block in memory

  printf("Tag %d\n", Tag);
  printf("Index %d\n", index);
  printf("MemAddress %d\n", MemAddress);

  CacheLine *Line = &L2Cache.lines[index];

  /* access Cache */

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      unsigned int OldMemAddress = ((Line->Tag << 9) + index) << 3;
      accessDRAM(OldMemAddress, &(L2CacheData[index * BLOCK_SIZE]),
                 MODE_WRITE); // then write back old block
    }

    memcpy(&(L2CacheData[index * BLOCK_SIZE]), TempBlock,
           BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(L2CacheData[index * BLOCK_SIZE]), BLOCK_SIZE);
    time += L2_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(L2CacheData[index * BLOCK_SIZE]), data, BLOCK_SIZE);
    time += L2_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void read(int address, unsigned char *data) {
  // printf("=== L1 BEFORE READ ===\n");
  // for (int i = 0; i < L1_SIZE; i++) {
  //       printf("%d ", L1CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");

  // printf("=== L2 BEFORE READ ===\n");
  // for (int i = 0; i < L2_SIZE; i++) {
  //       printf("%d ", L2CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");

  accessL1(address, data, MODE_READ);

  // printf("=== L1 AFTER READ ===\n");
  // for (int i = 0; i < L1_SIZE; i++) {
  //       printf("%d ", L1CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");
  
  // printf("=== L2 AFTER READ ===\n");
  // for (int i = 0; i < L2_SIZE; i++) {
  //       printf("%d ", L2CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");
}

void write(int address, unsigned char *data) {
  // printf("=== L1 BEFORE WRITE ===\n");
  // for (int i = 0; i < L1_SIZE; i++) {
  //       printf("%d ", L1CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");

  // printf("=== L2 BEFORE WRITE ===\n");
  // for (int i = 0; i < L2_SIZE; i++) {
  //       printf("%d ", L2CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");

  accessL1(address, data, MODE_WRITE);

  // printf("=== L1 AFTER WRITE ===\n");
  // for (int i = 0; i < L1_SIZE; i++) {
  //       printf("%d ", L1CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");

  // printf("=== L2 AFTER WRITE ===\n");
  // for (int i = 0; i < L2_SIZE; i++) {
  //       printf("%d ", L2CacheData[i]);

  //       if ((i + 1) % 8 == 0) {
  //         printf("\n");
  //       }
  // }
  // printf("\n");
}
