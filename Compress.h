#pragma once

#include "Supermarket.h"
typedef unsigned char BYTE;


void printCharAsBinary(BYTE ch);
BYTE createMask(int high, int low);
BYTE* compressedMarketDetailes(const SuperMarket* pMarket);
BYTE* compressedHouseNumber(const SuperMarket* pMarket);

int compressedProduct( Product* pProduct, FILE* f);
int readCompressedProduct(Product* pProduct, FILE* f);
int unCompressMarketDetailes(SuperMarket* pMarket, FILE* fp, int* productCount, int* nameLen);
char castingToWrite(char c);
char castingToRead(char c);
int unCompressedHouseNumber(const SuperMarket* pMarket, FILE* fp, int* houseNumber);