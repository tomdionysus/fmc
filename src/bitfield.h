#ifndef BITFIELD
#define BITFIELD 1

#include <stdint.h>
#include <stdbool.h>

typedef struct bitfield {
	uint32_t size;
	char *buffer;
} bitfield_t;

bitfield_t *bitfield_new(uint32_t size);
void bitfield_set(bitfield_t *bf, uint32_t pos);
void bitfield_clear(bitfield_t *bf, uint32_t pos);
void bitfield_toggle(bitfield_t *bf, uint32_t pos);
void bitfield_put(bitfield_t *bf, uint32_t pos, bool set);
bool bitfield_get(bitfield_t *bf, uint32_t pos);
void bitfield_reset(bitfield_t *bf);
void bitfield_destroy(bitfield_t *bf);

#endif // BITFIELD