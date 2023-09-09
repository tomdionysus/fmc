#include <stdlib.h>
#include <memory.h>

#include "bitfield.h"

bitfield_t *bitfield_new(uint32_t size) {
	bitfield_t *bf = (bitfield_t*)malloc(sizeof(bitfield_t));
	bf->size = size;
	bf->buffer = malloc(bf->size+(bf->size + 7) / 8);
	bitfield_reset(bf);
	return bf;
}

void bitfield_set(bitfield_t *bf, uint32_t pos) {
	bf->buffer[pos >> 3] |= 1 << (pos % 8); 
}

void bitfield_clear(bitfield_t *bf, uint32_t pos) {
	bf->buffer[pos >> 3] &= ~(1 << (pos % 8));
}

void bitfield_toggle(bitfield_t *bf, uint32_t pos) {
	bf->buffer[pos >> 3] ^= 1 << (pos % 8); 
}

void bitfield_put(bitfield_t *bf, uint32_t pos, bool set) {
	if(set) bitfield_set(bf,pos); else bitfield_clear(bf, pos);
}

bool bitfield_get(bitfield_t *bf, uint32_t pos) {
	return (bf->buffer[pos >> 3] & (1 << (pos % 8))) != 0;
}

void bitfield_reset(bitfield_t *bf) {
	memset(bf->buffer,0,(bf->size+(bf->size + 7) / 8));
}

void bitfield_destroy(bitfield_t *bf) {
	free(bf->buffer);
	free(bf);
}
