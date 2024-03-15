#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Compress.h"
#include "fileHelper.h"
#include "myMacros.h"

void printCharAsBinary(BYTE ch)
{
	int i;
	BYTE temp;
	for (i = 0; i < 8; i++)
	{
		temp = ch << i;
		temp = temp >> 7;
		printf("%d", (int)temp);
	}
}

BYTE createMask(int high, int low)
{
	return (1 << (high + 1)) - (1 << low);
}

BYTE* compressedMarketDetailes(const SuperMarket* pMarket)
{
	//Product Count [10]bit,name length [6]bit
	BYTE* buffer1 = (BYTE*)calloc(2, sizeof(BYTE));
	CHECK_RETURN_0(buffer1)

	//Product Count (8/10) 
	int productCount = getNumOfProductsInList(pMarket);
	buffer1[0] = productCount >> 2;

	//Product Count (2/10) + Market Name length (6/6)
	int len = (int)strlen(pMarket->name);
	buffer1[1] = ((productCount & 0x3) << 6) | ((len << 2) >> 2);

	return buffer1;
}


BYTE* compressedHouseNumber(const SuperMarket* pMarket)
{
	BYTE* buffer2 = (BYTE*)calloc(1, sizeof(BYTE));//house number[8]bit
	CHECK_RETURN_0(buffer2)


	buffer2[0] = pMarket->location.num;
	
	return buffer2;
}


int compressedProduct(Product* pProduct, FILE* f)
{
	int productNameLength = (int)strlen(pProduct->name);

	for (int i = 0; i < 7; i++)
	{
		pProduct->barcode[i] = castingToWrite(pProduct->barcode[i]);
	}


	BYTE data1[3];
	data1[0] = (pProduct->barcode[0] << 2) | (pProduct->barcode[1] >> 4);
	data1[1] = (pProduct->barcode[1] << 4) | ((pProduct->barcode[2] & 0x3C) >> 2);
	data1[2] = (pProduct->barcode[2] << 6) | pProduct->barcode[3];

	BYTE data2[3];
	data2[0] = (pProduct->barcode[4] << 2) | (pProduct->barcode[5] >> 6);
	data2[1] = (pProduct->barcode[5] << 4) | (pProduct->barcode[6] >> 2);
	data2[2] = (pProduct->barcode[6] << 6) | (productNameLength << 2) | pProduct->type;



	if (fwrite(&data1, sizeof(BYTE), 3, f) != 3)
		return 0;

	if (fwrite(&data2, sizeof(BYTE), 3, f) != 3)
		return 0;

	if (fwrite(pProduct->name, sizeof(char), productNameLength, f) != productNameLength)
		return 0;


	BYTE data3[3];
	data3[0] = pProduct->count;
	float floatPrice = pProduct->price;
	int shkalim, agorot;
	shkalim = (int)floatPrice;
	agorot = (int)((floatPrice - shkalim) * 100);
	data3[1] = (shkalim >> 8) | (agorot << 1);
	data3[2] = shkalim;

	if (fwrite(&data3, sizeof(BYTE), 3, f) != 3)
		return 0;

	return 1;


}


int readCompressedProduct(Product* pProduct, FILE* f) {


	BYTE data1[3];
	BYTE data2[3];
	if (fread(&data1, sizeof(BYTE), 3, f) != 3)
		return 0;

	if (fread(&data2, sizeof(BYTE), 3, f) != 3)
		return 0;

	pProduct->barcode[0] = (data1[0] >> 2);
	pProduct->barcode[1] = (((data1[0] & 0x03) << 4) | (data1[1] >> 4));
	pProduct->barcode[2] = (((data1[1] & 0x0F) << 2) | (data1[2] >> 6));
	pProduct->barcode[3] = (data1[2] & 0x3F);
	pProduct->barcode[4] = (data2[0] >> 2);
	pProduct->barcode[5] = ((data2[0] & 0x3) << 4 | (data2[1] & 0xF0) >> 4);
	pProduct->barcode[6] = ((data2[1] & 0xF) << 2 | (data2[2] >> 6));



	for (int i = 0; i < 7; i++)
	{
		pProduct->barcode[i] = castingToRead(pProduct->barcode[i]);
	}


	int nameLength;
	nameLength = (data2[2] & 0x3c) >> 2; //read name length
	pProduct->type = data2[2] & 0x3;

	if (fread(pProduct->name, sizeof(char), nameLength, f) != nameLength)
		return 0;


	BYTE data3[3];
	if (fread(&data3, sizeof(BYTE), 3, f) != 3)
		return 0;
	pProduct->count = data3[0];

	int agorot = (data3[1] >> 1);

	int Shekel = (data3[1] & 0x1) | (data3[2]);
	float fullPrice = (float)(Shekel+(agorot/100.0));
	pProduct->price = fullPrice;
	return 1;

}

char castingToWrite(char c) {
	if (c >= 65) {
		c -= 55;
	}
	else {
		c -= 48;
	}
	return c;
}

char castingToRead(char c)
{
	if (c >= 10) {
		c += 55;
	}
	else {
		c += 48;
	}
	return c;
}

int unCompressMarketDetailes(SuperMarket* pMarket, FILE* fp, int* productCount, int* nameLen)
{
	BYTE buf[2];
	*productCount = 0;
	*nameLen = 0;
	if (fread(&buf, sizeof(BYTE), 2, fp) != 2)
	{
		return 0;
	}
	*productCount = (buf[0] << 2) | (buf[1] >> 6);
	*nameLen = (buf[1] & 0x3f);
	return 1;

}

int unCompressedHouseNumber(const SuperMarket* pMarket, FILE* fp, int* houseNumber)
{
	BYTE buf[1];
	*houseNumber = 0;

	if (fread(&buf, sizeof(BYTE), 1, fp) != 1)
	{
		return 0;
	}
	*houseNumber = buf[0];
	return 1;
}
