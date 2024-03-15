#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Supermarket.h"
#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "myMacros.h"
#include "Compress.h"

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int isCompressed)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n")
		/*
		if (!fp) {
			printf("Error open supermarket file to write\n");
			return 0;
		}
		*/

		if (isCompressed) //writeSuperMarketDataToCompressedBFile
		{
			if (!writeSuperMarketDataToCompressedBFile(pMarket, fp))
			{
				return 0;
			}



		}
		else
		{
			if (!writeSuperMarketDataToBFile(pMarket, fp))
			{
				return 0;
			}
		}



	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (isCompressed) {
			if (!compressedProduct(pTemp, fp)) //writting products compressed
			{
				CLOSE_RETURN_0(fp)
			}
		}
		else {
			if (!saveProductToFile(pTemp, fp))
			{
				CLOSE_RETURN_0(fp)
			}
		}
		pN = pN->next;
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}
int writeSuperMarketDataToCompressedBFile(const SuperMarket* pMarket, FILE* fp)
{
	//writing number of products and MarketName len#####
	BYTE* superMarketData = compressedMarketDetailes(pMarket);
	if (fwrite(superMarketData, sizeof(BYTE), 2, fp) != 2)
		return 0;

	//writing Name of the market (char after char)#####
	int nameLen = (int)strlen(pMarket->name);
	if (fwrite(pMarket->name, sizeof(char), nameLen, fp) != nameLen)
		return 0;

	//writing Address###############################
	BYTE* houseNumberData = compressedHouseNumber(pMarket);
	if (fwrite(houseNumberData, sizeof(BYTE), 1, fp) != 1)
		return 0;

	//street
	int streetLen = (int)strlen(pMarket->location.street);
	if (!writeIntToFile(streetLen, fp, "Error write product count\n"))
	{
		CLOSE_RETURN_0(fp)
	}
	if (fwrite(pMarket->location.street, sizeof(char), streetLen, fp) != streetLen)
		return 0;

	//city
	int cityLen = (int)strlen(pMarket->location.city);
	if (!writeIntToFile(cityLen, fp, "Error write product count\n"))
	{
		CLOSE_RETURN_0(fp)
	}
	if (fwrite(pMarket->location.city, sizeof(char), cityLen, fp) != cityLen)
		return 0;
	return 1;
}
int writeSuperMarketDataToBFile(const SuperMarket* pMarket, FILE* fp)
{
	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		CLOSE_RETURN_0(fp)
	}

	if (!saveAddressToFile(&pMarket->location, fp))
	{
		CLOSE_RETURN_0(fp)
	}
	int count = getNumOfProductsInList(pMarket);

	if (!writeIntToFile(count, fp, "Error write product count\n"))
	{
		CLOSE_RETURN_0(fp)
	}
	return 1;
}

#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int compressed)
{
	int productCount = 0;
	int nameLength;
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp, "Error open company file\n")

		if (compressed) {
			unCompressMarketDetailes(pMarket, fp, &productCount, &nameLength);

			pMarket->name = (char*)calloc(nameLength + 1, sizeof(char));
			CHECK_RETURN_0(pMarket->name)
				if (fread(pMarket->name, sizeof(char), nameLength, fp) != nameLength) {
					fclose(fp);
					return 0;
				}
			int houseNumber;
			unCompressedHouseNumber(pMarket, fp, &houseNumber);
			pMarket->location.num = houseNumber;

			int streetLength;
			readIntFromFile(&streetLength, fp, "Error reading street length");
			pMarket->location.street = (char*)calloc(streetLength + 1, sizeof(char));
			CHECK_RETURN_0(pMarket->location.street);

			if (fread(pMarket->location.street, sizeof(char), streetLength, fp) != streetLength) {
				fclose(fp);
				return 0;
			}

			int cityLength;
			readIntFromFile(&cityLength, fp, "Error reading city length");
			pMarket->location.city = (char*)calloc(cityLength + 1, sizeof(char));
			CHECK_RETURN_0(pMarket->location.city);

			if (fread(pMarket->location.city, sizeof(char), cityLength, fp) != cityLength) {
				fclose(fp);
				return 0;
			}

		}
		else {



			pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
			if (!pMarket->name)
			{
				fclose(fp);
				return 0;
			}

			if (!loadAddressFromFile(&pMarket->location, fp))
			{
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

			}

			if (!readIntFromFile(&productCount, fp, "Error reading product count\n"))
			{
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

			}
		}

	if (productCount > 0)
	{
		Product* pTemp;
		for (int i = 0; i < productCount; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

			}
			if (compressed) {
				if (!(readCompressedProduct(pTemp, fp))) {
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

				}
			}
			else {
				if (!loadProductFromFile(pTemp, fp))
				{
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

				}
			}

			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

			}
		}
	}


	fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	(void)fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		(void)fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}