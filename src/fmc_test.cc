#include <gtest/gtest.h>

extern "C" {
  #include "fmc.h"
}

char storageBuffer[65536];

uint8_t test_read(uint32_t block, char* buffer) {
	memcpy(buffer, (char*)(storageBuffer+(block*4096)), 4096);
	return (uint8_t)4096;
}

uint8_t test_write(uint32_t block, char* buffer) {
	memcpy((char*)(storageBuffer+(block*4096)), buffer, 4096);
	return (uint8_t)4096;
}

// Instantiate, check 
TEST(fmc, fmc_new_destroy) {
	fmc_t *cache = fmc_new(16, 4096, 256, &test_read, &test_write);

	fmc_destroy(cache);
}

// Macros

TEST(fmc, FMC_GET_START_PAGE_FROM_PAGE) {
	fmc_t mock;

	mock.blockSize = 4096;
	mock.pageSize = 256; 

	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE(&mock, 0),0);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE(&mock, 4),0);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE(&mock, 7),0);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE(&mock, 20),16);

	mock.blockSize = 1024;
	mock.pageSize = 256; 

	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE((&mock), 0),0);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE((&mock), 4),4);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE((&mock), 7),4);
	EXPECT_EQ(FMC_GET_START_PAGE_FROM_PAGE((&mock), 20),20);
}

TEST(fmc, FMC_GET_BLOCK_NUMBER_FROM_PAGE) {
	fmc_t mock;

	mock.blockSize = 4096;
	mock.pageSize = 512; 

	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 0),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 4),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 7),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 10),1);

	mock.blockSize = 8192;
	mock.pageSize = 128; 

	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 0),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 4),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 63),0);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 64),1);
	EXPECT_EQ(FMC_GET_BLOCK_NUMBER_FROM_PAGE(&mock, 128),2);
}

TEST(fmc, FMC_IS_IN_BLOCK) {
	fmc_t testFmc;

	testFmc.blockSize = 8192;
	testFmc.pageSize = 128; 

	fmc_block_t mock0; mock0.startPage = 0;
	fmc_block_t mock1; mock1.startPage = 128;

	EXPECT_TRUE(FMC_IS_IN_BLOCK(&testFmc, &mock0, 0));
	EXPECT_TRUE(FMC_IS_IN_BLOCK(&testFmc, &mock0, 4));
	EXPECT_TRUE(FMC_IS_IN_BLOCK(&testFmc, &mock0, 63));
	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock0, 64));

	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 0));
	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 4));
	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 127));

	EXPECT_TRUE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 128));
	EXPECT_TRUE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 191));
	
	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 192));
	EXPECT_FALSE(FMC_IS_IN_BLOCK(&testFmc, &mock1, 2048));
}

// Internals

TEST(fmc, _fmc_get_oldest_block) {
	fmc_t testFmc; testFmc.blockCount = 4;

	fmc_block_t *blocks[4];
	testFmc.blocks = blocks; 

	fmc_block_t mock0; mock0.lastAccess = 71263123;
	fmc_block_t mock1; mock1.lastAccess = 2342342;
	fmc_block_t mock2; mock2.lastAccess = 71263120;
	fmc_block_t mock3; mock3.lastAccess = 71260000;

	testFmc.blocks[0] = &mock0;
	testFmc.blocks[1] = &mock1;
	testFmc.blocks[2] = &mock2;
	testFmc.blocks[3] = &mock3;

	EXPECT_EQ(_fmc_get_oldest_block(&testFmc), &mock1);

	testFmc.blocks[0] = nullptr;
	testFmc.blocks[1] = nullptr;
	testFmc.blocks[2] = nullptr;
	testFmc.blocks[3] = &mock3;

	EXPECT_EQ(_fmc_get_oldest_block(&testFmc), &mock3);

	testFmc.blocks[3] = nullptr;

	EXPECT_EQ(_fmc_get_oldest_block(&testFmc), nullptr);

	testFmc.blockCount = 0;

	EXPECT_EQ(_fmc_get_oldest_block(&testFmc), nullptr);
}

TEST(fmc, _fmc_get_block_for_page) {
	fmc_t testFmc; 
	testFmc.blockCount = 4;
	testFmc.blockSize = 4096;
	testFmc.pageSize = 256; 

	fmc_block_t *blocks[4];
	testFmc.blocks = blocks; 

	testFmc.blocks[0] = nullptr;
	testFmc.blocks[1] = nullptr;
	testFmc.blocks[2] = nullptr;
	testFmc.blocks[3] = nullptr;

	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 0), nullptr);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 16), nullptr);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 1239812983), nullptr);

	fmc_block_t mock0; mock0.startPage = 0;
	fmc_block_t mock1; mock1.startPage = 16;
	fmc_block_t mock2; mock2.startPage = 128;
	
	testFmc.blocks[0] = &mock0;
	testFmc.blocks[1] = &mock1;
	testFmc.blocks[2] = &mock2;
	testFmc.blocks[3] = nullptr;

	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 0), &mock0);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 5), &mock0);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 16), &mock1);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 31), &mock1);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 32), nullptr);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 127), nullptr);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 128), &mock2);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 144), nullptr);
	EXPECT_EQ(_fmc_get_block_for_page(&testFmc, 1284827348), nullptr);
}

TEST(fmc, _fmc_create_block) {
	fmc_t *testFmc = fmc_new(6, 4096, 256, nullptr, nullptr); 

	fmc_block_t *test0 = _fmc_create_block(testFmc);
	fmc_block_t *test1 = _fmc_create_block(testFmc);
	fmc_block_t *test2 = _fmc_create_block(testFmc);
	fmc_block_t *test3 = _fmc_create_block(testFmc);
	fmc_block_t *test4 = _fmc_create_block(testFmc);
	fmc_block_t *test5 = _fmc_create_block(testFmc);
	fmc_block_t *test6 = _fmc_create_block(testFmc);

	EXPECT_NE(test0, nullptr);
	EXPECT_NE(test1, nullptr);
	EXPECT_NE(test2, nullptr);
	EXPECT_NE(test3, nullptr);
	EXPECT_NE(test4, nullptr);
	EXPECT_NE(test5, nullptr);

	EXPECT_EQ(test6, nullptr);

	fmc_destroy(testFmc);
}

// Main
