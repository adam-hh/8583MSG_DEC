/*---------------------
Author HuHui(Adam)
8583 functions all here
---------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"
/*-------------------
BCD decoder, from memory *src to char *des, when *src high 4 bits or low 4 bits in [0xA,0xF], decode fail.
l is the length of *src, memory piece size.
-------------------*/
int BCDDecode(const u8 *src, char *des, size_t l)
{
	u8 valh, vall;
	for( size_t i = 0; i < l; i++)
		{
			valh = (*(src + i) & 0xF0) >> 4;
			vall = *(src + i) & 0x0F;
			if(valh < 0x0A)
				*(des + (i*2)) = valh + 0x30;
			else
				{
					printf("the byte is not in BCD coding range at index %u.\n", i);
					return FAIL;
				}
			if(vall < 0x0A)
				*(des + (i*2) + 1) = vall + 0x30;
			else
				{
					printf("the byte is not in BCD coding range at index %u.\n", i);
					return FAIL;
				}
				
		}
	return OK;
}

/*-------------------
BCD encoder, from char *src to memory *des,when *src is not in ['0','9'], encode fail.
l is the length of *des, memory piece size.
-------------------*/
int BCDEncode(const char *src, u8 *des, size_t l)
{
	size_t src_l = strlen(src);
	if(src_l != ((2*l)+1))
		{
			printf("illegal hex view string size or memory size!\n");
			return FAIL;
		}
	u8 valh, vall;
	for(size_t i =0; i < l; i++)
		{
			if(((*(src + (2*i))) > 0x29) && ((*(src + (2*i))) < 0x40)) 
				valh = ((*(src + (2*i)) - 0x30) << 4);
			else
				{
					printf("the char %c at index %u is not in BCD coding range.\n", *(src + (2*i)), (2*i));
					return FAIL;
				}				
			if(((*(src + (2*i) + 1)) > 0x29) && ((*(src + (2*i) +1)) < 0x40)) 
					vall = (*(src + (2*i) +1) - 0x30);
			else
				{
					printf("the char %c at index %u is not in BCD coding range.\n", *(src + (2*i) +1), ((2*i) +1));
					return FAIL;
				}
			*(des + i) = (valh + vall);				
		}
	return OK;
}

/*-------------------
Calculate the number in memory *src, l is length of *src, memory piece size.
align is the numbers align, can be RIGHT or LEFT.
actually left align has a logical error by normal explain.
-------------------*/
unsigned int BCDCal(u8 *src, size_t l, HowNumberAlign align)
{
	u8 valh, vall;
	unsigned int rlt = 0;
	for( size_t i = 0; i < l; i++)
		{
			valh = (*(src + i) & 0xF0) >> 4;
			vall = *(src + i) & 0x0F;
			unsigned int valh_int = valh;
			unsigned int vall_int = vall;
			if(valh < 0x0A)
				if(align == RIGHT)
					{
						int flag_n_h_0 = 0; // flag to non o at high bits of byte
						if((valh != 0) || flag_n_h_0)
							{
								for(size_t j = 0; j < ((2*l) - (2*i) -1); j++)
								{
									valh_int *= 10;								
								}
								rlt += valh_int;
								flag_n_h_0 = 1;
							} else
								{
									;
								}
						
					} else// align == LEFT
						{
							for(size_t j = 0; j < (2*i); j++)
							{
								valh_int *= 10;								
							}
							rlt += valh_int;
						}
			else
				{
					printf("the byte is not in BCD coding range at index %u.\n", i);
					return FAIL;
				}
			if(vall < 0x0A)
				if(align == RIGHT)
					{
						int flag_n_l_0 = 0; // flag to non o at low bits of byte
						if((vall != 0) || flag_n_l_0)
							{
								for(size_t j = 0; j < ((2*l) - (2*i) -2); j++)
								{
									vall_int *= 10;								
								}
								rlt += vall_int;
								flag_n_l_0 = 1;
							}						
					} else //align == LEFT;
						{
							for(size_t j = 0; j < ((2*i) + 1); j++)
							{
								valh_int *= 10;								
							}
							rlt += valh_int;
						}
			else
				{
					printf("the byte is not in BCD coding range at index %u.\n", i);
					return FAIL;
				}
				
		}
	return OK;
}

/*-------------------
The memory *src to char *des, l is the *src length, as the function name, just to hex view.
-------------------*/
int Hexview(const u8 *src, char *des, size_t l)
{
	u8 valh, vall;
	for( size_t i = 0; i < l; i++)
		{
			valh = (*(src + i) & 0xF0) >> 4;
			vall = *(src + i) & 0x0F;
			if(valh < 0x0A)
				*(des + (i*2)) = valh + 0x30;
			else
				switch(valh)
					{
						case 0x0A: *(des + (i*2)) = 0x61;break;
						case 0x0B: *(des + (i*2)) = 0x62;break;
						case 0x0C: *(des + (i*2)) = 0x63;break;
						case 0x0D: *(des + (i*2)) = 0x64;break;
						case 0x0E: *(des + (i*2)) = 0x65;break;
						case 0x0F: *(des + (i*2)) = 0x66;break;
						default: return FATAL_ERROR ;
					}
			if(vall < 0x0A)
				*(des + (i*2) + 1) = vall + 0x30;
			else
				switch(vall)
					{
						case 0x0A: *(des + (i*2) +1) = 0x61;break;
						case 0x0B: *(des + (i*2) +1) = 0x62;break;
						case 0x0C: *(des + (i*2) +1) = 0x63;break;
						case 0x0D: *(des + (i*2) +1) = 0x64;break;
						case 0x0E: *(des + (i*2) +1) = 0x65;break;
						case 0x0F: *(des + (i*2) +1) = 0x66;break;
						default: return FATAL_ERROR;
					}
		}
	return OK;
}

/*-------------------
The char *src to memory *des, *src is the hex view string of some piece of memory, l is the *des length.
-------------------*/
int HexviewcharToHex(const char *src, u8 *des, size_t l)
{
	size_t src_l = strlen(src);
	if(src_l != ((2*l)+1))
		{
			printf("illegal hex view string size or memory size!\n");
			return FAIL;
		}
	u8 valh, vall;
	for(size_t i =0; i < l; i++)
		{
			if(((*(src + (2*i))) > 0x29) && ((*(src + (2*i))) < 0x40)) 
				valh = ((*(src + (2*i)) - 0x30) << 4);
			else
				switch(*(src + (2*i)))
					{
						case 'A': ;
						case 'a': valh = 0x0A << 4;break;
						case 'B': ;
						case 'b': valh = 0x0B << 4;break;
						case 'C': ;
						case 'c': valh = 0x0C << 4;break;
						case 'D': ;
						case 'd': valh = 0x0D << 4;break;
						case 'E': ;
						case 'e': valh = 0x0E << 4;break;
						case 'F': ;
						case 'f': valh = 0x0F << 4;break;
						default: printf("illegal hex char %c at index %u\n", *(src + (2*i)), (2*i));return FAIL;
					}
				if(((*(src + (2*i) + 1)) > 0x29) && ((*(src + (2*i) +1)) < 0x40)) 
					vall = (*(src + (2*i) +1) - 0x30);
				else
					switch(*(src + (2*i) +1))
					{
						case 'A': ;
						case 'a': vall = 0x0A;break;
						case 'B': ;
						case 'b': vall = 0x0B;break;
						case 'C': ;
						case 'c': vall = 0x0C;break;
						case 'D': ;
						case 'd': vall = 0x0D;break;
						case 'E': ;
						case 'e': vall = 0x0E;break;
						case 'F': ;
						case 'f': vall = 0x0F;break;
						default: printf("illegal hex char %c at index %u\n", *(src + (2*i) + 1), (2*i) + 1);return FAIL;
					}
				*(des + i) = (valh + vall);
		}
}


