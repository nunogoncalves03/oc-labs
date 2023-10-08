#include "L1Cache.h"

uint8_t L1CacheData[L1_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache L1Cache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

  if (address >= DRAM_SIZE - BLOCK_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;

    printf("=== RAM READ %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", DRAM[((address >> OFFSET_BITS) << OFFSET_BITS) + i]);
    }
    printf("\n");
  }

  if (mode == MODE_WRITE) {
    printf("=== RAM BEFORE WRITE %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", DRAM[((address >> OFFSET_BITS) << OFFSET_BITS) + i]);
    }
    printf("\n");

    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;

    printf("=== RAM AFTER WRITE %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", DRAM[((address >> OFFSET_BITS) << OFFSET_BITS) + i]);
    }
    printf("\n");
  }
}

/*********************** L1 cache *************************/

void initCache() { L1Cache.init = 0; }

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  // 18 bits tag, 8 bits index, 6 bits offset
  uint32_t offset, index, Tag, MemAddress;
  uint8_t TempBlock[BLOCK_SIZE];

  /* init cache */
  if (L1Cache.init == 0) {
    L1Cache.init = 1;

    for (uint32_t i = 0; i < L1_LINES_N; i++) {
      L1Cache.lines[i].Valid = 0;
    }
  }

  offset = address & OFFSET_MASK; // apply a mask to remove the tag and index

  index = address >> OFFSET_BITS; // remove offset
  index = index & L1_INDEX_MASK; // apply a mask to remove the tag

  Tag = address >> (L1_INDEX_BITS + OFFSET_BITS); // remove index and offset

  MemAddress = (address >> OFFSET_BITS) << OFFSET_BITS; // address of the block in memory

  printf("Tag %d\n", Tag);
  printf("Offset %d\n", offset);
  printf("Index %d\n", index);
  printf("MemAddress %d\n", MemAddress);

  CacheLine *Line = &L1Cache.lines[index];

  /* access Cache*/

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      uint32_t OldMemAddress = ((Line->Tag << L1_INDEX_BITS) + index) << OFFSET_BITS;
      accessDRAM(OldMemAddress, &(L1CacheData[index * BLOCK_SIZE]), MODE_WRITE); // then write back old block
    }

    memcpy(&(L1CacheData[index * BLOCK_SIZE]), TempBlock,
           BLOCK_SIZE); // copy new block to cache line

    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    printf("=== L1 READ %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", L1CacheData[index * BLOCK_SIZE + i]);
    }
    printf("\n");

    // assuming the given address is aligned
    memcpy(data, &(L1CacheData[index * BLOCK_SIZE + offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    printf("=== L1 BEFORE WRITE %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", L1CacheData[index * BLOCK_SIZE + i]);
    }
    printf("\n");

    // assuming the given address is aligned
    memcpy(&(L1CacheData[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;

    printf("=== L1 AFTER WRITE %d ===\n", address);
    for (int i = 0; i < BLOCK_SIZE / WORD_SIZE; i++) {
          printf("%d ", L1CacheData[index * BLOCK_SIZE + i]);
    }
    printf("\n");
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
