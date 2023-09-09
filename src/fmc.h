#ifndef FMC
#define FMC 1

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "bitfield.h"

#define FMC_PAGE_SIZE 256
#define FMC_BLOCK_SIZE 4096
#define FMC_CACHE_BLOCKS 4

#define FMC_PAGES_PER_BLOCK(F)  			 ((F)->blockSize / (F)->pageSize)
#define FMC_GET_BLOCK_NUMBER_FROM_PAGE(F,P)  ((P) / FMC_PAGES_PER_BLOCK((F)))
#define FMC_GET_START_PAGE_FROM_PAGE(F,P)    (((P) / FMC_PAGES_PER_BLOCK((F)) * FMC_PAGES_PER_BLOCK((F))))
#define FMC_IS_IN_BLOCK(F,B,P)              ((B) && ((P) >= (B)->startPage) && ((P) < (B)->startPage + ((F)->blockSize / (F)->pageSize)))


typedef struct fmc_block {
	time_t lastAccess;
	uint32_t startPage;
	bool dirty;
	char *buffer;
} fmc_block_t;

typedef uint8_t (*fmc_read_block_fn)(uint32_t block, char* buffer);
typedef uint8_t (*fmc_write_block_fn)(uint32_t block, char* buffer);

typedef struct fmc {
	uint8_t blockCount;
	uint32_t blockSize;
	uint32_t pageSize;

	fmc_read_block_fn readFn;
	fmc_write_block_fn writeFn;

	fmc_block_t **blocks;
} fmc_t;

// Instantiation
fmc_t *fmc_new(uint8_t blockCount, uint32_t blockSize, uint32_t pageSize, fmc_read_block_fn readFn, fmc_write_block_fn writeFn);
void fmc_destroy(fmc_t *testFmc);

// Main Interface
void fmc_read_page(fmc_t *testFmc, uint32_t page, char *buffer);
void fmc_write_page(fmc_t *testFmc, uint32_t page, char *buffer);
void fmc_flush_all(fmc_t *testFmc);

// Interals
fmc_block_t *_fmc_create_or_flush_oldest_block_and_read_page(fmc_t *fmc, uint32_t page);
fmc_block_t *_fmc_create_block(fmc_t *testFmc);
fmc_block_t *_fmc_get_block_for_page(fmc_t *testFmc, uint32_t page);
fmc_block_t *_fmc_get_oldest_block(fmc_t *testFmc);

#endif // FMC