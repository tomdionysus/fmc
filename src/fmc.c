#include <memory.h>

#include "fmc.h"
#include "bitfield.h"

fmc_t *fmc_new(uint8_t blockCount, uint32_t blockSize, uint32_t pageSize, fmc_read_block_fn readFn, fmc_write_block_fn writeFn) {
	fmc_t *fmc = (fmc_t*)calloc(sizeof(fmc_t), 1);
	
	// Setup config
	fmc->blockCount = blockCount;
	fmc->blockSize = blockSize;
	fmc->pageSize = pageSize;

	// Assign IO functions
	fmc->readFn = readFn;
	fmc->writeFn = writeFn;

	// Allocate and clear the block list pointers
	fmc->blocks = calloc(blockCount, sizeof(fmc_block_t*));

	// Return the fmc	
	return fmc;
}

void fmc_destroy(fmc_t *fmc) {
	// Free blocks if created
	for(uint8_t i=0; i<fmc->blockCount; i++) {
		if(fmc->blocks[i]) {
			free(fmc->blocks[i]->buffer);
			free(fmc->blocks[i]);
		}
	}
	// Free block list pointers
	free(fmc->blocks);

	// Free fmc
	free(fmc);
}

// Main

void fmc_read_page(fmc_t *fmc, uint32_t page, char *buffer) {
	// Do we have the block for page in cache?
	fmc_block_t *block = _fmc_get_block_for_page(fmc, page);

	// If the block is not in cache...
	if(!block) {
		block = _fmc_create_or_flush_oldest_block_and_read_page(fmc, page);
	}

	// Do the read into to the caller's buffer
	memcpy(buffer, block->buffer + (page-block->startPage) * fmc->pageSize, fmc->pageSize);

	// Mark lastAccess now
	block->lastAccess = time(NULL);
}

void fmc_write_page(fmc_t *fmc, uint32_t page, char *buffer) {
	// Do we have the block for page in cache?
	fmc_block_t *block = _fmc_get_block_for_page(fmc, page);

	if(!block) {
		block = _fmc_create_or_flush_oldest_block_and_read_page(fmc, page);
	}

	// Do the write from to the caller's buffer
	memcpy(block->buffer + (page-block->startPage) * fmc->pageSize, buffer, fmc->pageSize);

	// Mark block dirty and lastAccess now
	block->lastAccess = time(NULL);
	block->dirty = true;
}

void fmc_flush_all(fmc_t *fmc) {
	// Iterate all blocks...
	for(uint8_t i=0; i<fmc->blockCount; i++) {
		fmc_block_t *block = fmc->blocks[i];

		// Flush if loaded and dirty
		if(block && block->dirty) {
			// Write the block to the underlying storage
			fmc->writeFn(FMC_GET_BLOCK_NUMBER_FROM_PAGE(fmc, block->startPage), block->buffer);

			// Mark block dirty and lastAccess now
			block->lastAccess = time(NULL);
			block->dirty = false;
		}
	}
}

// Internals
fmc_block_t *_fmc_create_or_flush_oldest_block_and_read_page(fmc_t *fmc, uint32_t page) {
	// Try to create a cache block
	fmc_block_t *block = _fmc_create_block(fmc);

	// If we don't have any free blocks...
	if(!block) {
		// Get the oldest block
		block = _fmc_get_oldest_block(fmc);
		// If dirty, flush it
		if(block->dirty) {
			fmc->writeFn(FMC_GET_BLOCK_NUMBER_FROM_PAGE(fmc, block->startPage), block->buffer);
		}
	}

	// Reset the block's start page to the first page in the requested page block
	block->startPage = FMC_GET_START_PAGE_FROM_PAGE(fmc, page);

	// Read the requested block into this buffer, set clean
	fmc->readFn(FMC_GET_BLOCK_NUMBER_FROM_PAGE(fmc, page), block->buffer);
	block->dirty = false;

	return block;
}

fmc_block_t *_fmc_get_block_for_page(fmc_t *fmc, uint32_t page) {
	for(uint8_t i=0; i<fmc->blockCount; i++) {
		if(fmc->blocks[i] && FMC_IS_IN_BLOCK(fmc, fmc->blocks[i], page)) return fmc->blocks[i];
	}

	return NULL;
}

fmc_block_t *_fmc_create_block(fmc_t *fmc) {
	for(uint8_t i=0; i<fmc->blockCount; i++) {
		if(!fmc->blocks[i]) {
			// There is, create a bloack.
			fmc->blocks[i] = malloc(sizeof(fmc_block_t));
			fmc->blocks[i]->buffer = malloc(fmc->blockSize);
			return fmc->blocks[i];
		}
	}

	return NULL;
}


fmc_block_t *_fmc_get_oldest_block(fmc_t *fmc) {
	time_t lastAccess = 0xFFFFFFFF;
	fmc_block_t *oldest = NULL;

	for(uint8_t i=0; i<fmc->blockCount; i++) {
		if(fmc->blocks[i] && fmc->blocks[i]->lastAccess < lastAccess) {
			lastAccess = fmc->blocks[i]->lastAccess;
			oldest = fmc->blocks[i];
		}
	}

	return oldest;
}
