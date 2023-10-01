#define WORD_SIZE 4                 // in bytes, i.e 32 bit words
#define BLOCK_SIZE 2 * WORD_SIZE    // in bytes, 8 bytes = 64 bits
#define DRAM_SIZE 1024 * BLOCK_SIZE // in bytes, 8 kB
#define L1_SIZE 32 * BLOCK_SIZE     // in bytes, 256 bytes
#define L2_SIZE 512 * BLOCK_SIZE    // in bytes, 4 kB

#define MODE_READ 1
#define MODE_WRITE 0

#define DRAM_READ_TIME 100
#define DRAM_WRITE_TIME 50
#define L2_READ_TIME 10
#define L2_WRITE_TIME 5
#define L1_READ_TIME 1
#define L1_WRITE_TIME 1