#include <gtest/gtest.h>

extern "C" {
  #include "bitfield.h"
}

// Instantiate, check 
TEST(Bitfield, NewDestroy) {
  bitfield_t *bf = bitfield_new(16);

  EXPECT_EQ(bf->size,16);

  for(uint8_t i = 0; i<2; i++) {
    EXPECT_EQ(bf->buffer[i], 0);
  }

  bitfield_destroy(bf);
}

TEST(Bitfield, SetClearToggle) {
  bitfield_t *bf = bitfield_new(14);

  bitfield_set(bf, 1);
  bitfield_set(bf, 2);
  bitfield_set(bf, 3);
  bitfield_set(bf, 5);
  bitfield_set(bf, 8);
  bitfield_set(bf, 13);

  bitfield_set(bf, 10);
  bitfield_set(bf, 11);
  bitfield_set(bf, 12);

  bitfield_clear(bf, 10);
  bitfield_clear(bf, 11);

  bitfield_toggle(bf, 12);

  EXPECT_EQ(bf->buffer[0], 0b00101110);
  EXPECT_EQ(bf->buffer[1], 0b00100001);

  bitfield_destroy(bf);
}

TEST(Bitfield, Get) {
  bitfield_t *bf = bitfield_new(14);

  bf->buffer[0] = 0b00101110;
  bf->buffer[1] = 0b00100001;

  EXPECT_EQ(bitfield_get(bf,0), false);
  EXPECT_EQ(bitfield_get(bf,1), true);
  EXPECT_EQ(bitfield_get(bf,2), true);
  EXPECT_EQ(bitfield_get(bf,3), true);
  EXPECT_EQ(bitfield_get(bf,4), false);
  EXPECT_EQ(bitfield_get(bf,5), true);
  EXPECT_EQ(bitfield_get(bf,6), false);
  EXPECT_EQ(bitfield_get(bf,7), false);

  EXPECT_EQ(bitfield_get(bf,8), true);
  EXPECT_EQ(bitfield_get(bf,9), false);
  EXPECT_EQ(bitfield_get(bf,10), false);
  EXPECT_EQ(bitfield_get(bf,11), false);
  EXPECT_EQ(bitfield_get(bf,12), false);
  EXPECT_EQ(bitfield_get(bf,13), true);
  EXPECT_EQ(bitfield_get(bf,14), false);
  EXPECT_EQ(bitfield_get(bf,15), false);

  bitfield_destroy(bf);
}

TEST(Bitfield, Reset) {
  bitfield_t *bf = bitfield_new(14);

  bitfield_set(bf, 1);
  bitfield_set(bf, 2);
  bitfield_set(bf, 3);
  bitfield_set(bf, 5);
  bitfield_set(bf, 8);
  bitfield_set(bf, 13);

  bitfield_reset(bf);

  EXPECT_EQ(bf->buffer[0], 0);
  EXPECT_EQ(bf->buffer[1], 0);

  bitfield_destroy(bf);
}