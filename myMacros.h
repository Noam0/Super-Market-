#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define CHECK_RETURN_0(ptr){\
	if (!(ptr)){\
	return 0;\
	}\
}// used 2 times in supermarket.c

#define CHECK_MSG_RETURN_0(ptr, msg) {\
	if(!(ptr)){\
		printf("%s\n", msg);\
		return 0;\
	}\
}//used 2 time's when fopen didnt work

#define FREE_CLOSE_FILE_RETURN_0(ptr ,file) {\
	free(ptr);\
	fclose(file);\
	return 0;\
}//used 4 times

#define CLOSE_RETURN_0(file) {\
	fclose(file); \
	return 0;\
}//used 4 times

