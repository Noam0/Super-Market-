#pragma once

#include <stdio.h>
#include "Supermarket.h"

int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
			const char* customersFileName, int isCompressed);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
			const char* customersFileName, int compressed);

//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);


//yosi:
int writeSuperMarketDataToBFile(const SuperMarket* pMarket, FILE* fp);
int writeSuperMarketDataToCompressedBFile(const SuperMarket* pMarket, FILE* fp);