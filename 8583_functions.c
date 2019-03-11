/*---------------------
Author HuHui(Adam)
8583 functions all here
---------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"
/*-------------------
BCD decoder, from memory *src to char *des, when *src high 4 bits or low 4 bits in
[0xA,0xF], decode fail.
-------------------*/
int BCDDecode(u8 *src, char *des)
{
	
}

/*-------------------
BCD encoder, from char *src to memory *des,when *src is not in ['0','9'], encode fail
-------------------*/
int BCDEncode(char *src, u8 *des)
{
	
}

/*-------------------
Calculate the number in memory *src, len is the number field's width, 
align is the align way when the actual number's width is less than number field's width.
-------------------*/
unsigned int BCDCal(u8 *src, int len, HowNumberAlign align)
{
	
}

/*-------------------

-------------------*/
int Hexview(u8 *src, char *des)
{
	
}

/*-------------------

-------------------*/
int HexviewcharToHex(char *src, u8 *des)
{
	
}


