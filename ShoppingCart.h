#pragma once

#include "ShoppingItem.h"

#define DETAIL_PRINT

#ifdef DETAIL_PRINT
#define PRINT_TOTAL(price) printf("Total bill to pay: %.2f\n", price);
#else
#define PRINT_TOTAL(price)
#endif


typedef struct
{
	ShoppingItem**	itemArr;
	int				count;
}ShoppingCart;


void	initCart(ShoppingCart* pCart);
float	getTotalPrice(const ShoppingCart* pCart);
ShoppingItem*	getItemByBarocde(ShoppingCart* pCart, const char* barcode);
int		addItemToCart(ShoppingCart* pCart, const char* name, float price, int count);
float	printShoppingCart(const ShoppingCart* pCart);

void	freeShoppingCart(ShoppingCart* pCart);