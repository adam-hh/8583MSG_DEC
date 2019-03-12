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
					printf("the byte is not in BCD coding range at index %lu.\n", i);
					return FAIL;
				}
			if(vall < 0x0A)
				*(des + (i*2) + 1) = vall + 0x30;
			else
				{
					printf("the byte is not in BCD coding range at index %lu.\n", i);
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
					printf("the char %c at index %lu is not in BCD coding range.\n", *(src + (2*i)), (2*i));
					return FAIL;
				}				
			if(((*(src + (2*i) + 1)) > 0x29) && ((*(src + (2*i) +1)) < 0x40)) 
					vall = (*(src + (2*i) +1) - 0x30);
			else
				{
					printf("the char %c at index %lu is not in BCD coding range.\n", *(src + (2*i) +1), ((2*i) +1));
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
unsigned int BCDCal(const u8 *src, size_t l, HowNumberAlign align)
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
					printf("the byte is not in BCD coding range at index %lu.\n", i);
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
					printf("the byte is not in BCD coding range at index %lu.\n", i);
					return FAIL;
				}
				
		}
	return rlt;
}

/*----------------------

----------------------*/
unsigned int BCDCalInMsgLenField(const u8 *src, size_t l, HowNumberAlign align)
{
	unsigned int rlt = BCDCal(src, l, align);
	if((rlt % 2) == 0)
		return rlt;
	else
		return (rlt + 1);
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
	if(src_l < (2*l))
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
						default: printf("illegal hex char %c at index %lu\n", *(src + (2*i)), (2*i));return FAIL;
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
						default: printf("illegal hex char %c at index %lu\n", *(src + (2*i) + 1), (2*i) + 1);return FAIL;
					}
				*(des + i) = (valh + vall);
		}
}

/*-------------------
Calculate the value of *src of memory, l less then 5
-------------------*/
unsigned int CalSmalMemoryPieceVal(const u8 *src, size_t l)
{
	unsigned int rlt = 0;
	for(size_t i = 0; i < l; i++)
	{
		rlt += ((*(src + i)) << (l -i -1));
	}
	return rlt;
}

/*-----------------------------
Test the bitmap(*src), if the index sequnce of bit in memory *src matched 1, test sucess.
else test fail.
-----------------------------*/
int BitMaptest(const int index, const u8 *src, size_t l)
{
	if((index < 1) || (index > ((8*l) +1)))
		{
			printf("ivalid index %d or bitmap length %lu \n", index, l);
			return FATAL_ERROR;
		}
	u8 *Mask = (char*)malloc(l);
	memset(Mask, 0, l);
	int mod = index % 8;
	u8 chr = 0x0;
	switch(mod)
	{
		case 1: chr = 0x80;break;
		case 2: chr = 0x40;break;
		case 3: chr = 0x20;break;
		case 4: chr = 0x10;break;
		case 5: chr = 0x8;break;
		case 6: chr = 0x4;break;
		case 7: chr = 0x2;break;
		case 0: chr = 0x1;break;
		default:printf("bitmap index error \n"); return FATAL_ERROR;
	}
	int i = ((index -1) / 8);
	*(Mask + i) = chr;

	for(size_t j = 0; j < l; j++)
		{
			if((*(Mask + j)) & (*(src + j)))
				return OK;
		}
	free(Mask);
	return FAIL;
}

/*------------------------------
Decode the message in memory *src.
Set the decode result in ptrdecoder.
This is the decode engine.
------------------------------*/
int DecodeMemMsg(const u8* src, PtrDecoder ptrdecoder, size_t l)
{
	memset(ptrdecoder, 0, sizeof(Decoder)); // init ptrdecoder

	ptrdecoder->Msgmem = src;
	ptrdecoder->Msgmem_l = l;
	int curpos = 0;  // current decoder engine offset
	ptrdecoder->FieldLen = 0;
	curpos += ((ptrdecoder->FieldLen_l) = 2);
	ptrdecoder->FieldTPDU = curpos;
	curpos += ((ptrdecoder->FieldTPDU_l) = 5);
	ptrdecoder->FieldMsghead = curpos;
	curpos += ((ptrdecoder->FieldMsghead_l) = 6);
	
	ptrdecoder->Field0 = curpos; //This is a length var field, but length field always 0x04, just like a fixed length field.
	ptrdecoder->Field0_l = (1 + ((BCDCalInMsgLenField((src + curpos), 1, RIGHT) / 2)));
	curpos += (ptrdecoder->Field0_l);

	ptrdecoder->Field1 = curpos; //  Field1 is actuall the bitmap
	if((*(src + curpos)) > 0x80)
		ptrdecoder->Field1_l = 16;
	else
		ptrdecoder->Field1_l = 8;
	curpos += (ptrdecoder->Field1_l);
	size_t BitMapLen = ptrdecoder->Field1_l;
	u8 *BitMap = (u8*)malloc(BitMapLen);
	memcpy(BitMap, src + curpos, BitMapLen);
	
#define TEST(x) BitMaptest(x, BitMap, BitMapLen)
	int rlt = FAIL;           // Field2
	rlt = TEST(2);
	if(OK == rlt)
		{
			ptrdecoder->Field2 = curpos;
			ptrdecoder->Field2_l = (1 + (BCDCalInMsgLenField((src + curpos), 1, RIGHT)/2));
			curpos += (ptrdecoder->Field2_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field2 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(3);  //Field3
	if(OK == rlt)
		{
			ptrdecoder->Field3 = curpos;
			curpos += (ptrdecoder->Field3_l = 3);
		}
	else if(FAIL == rlt)
		{
			printf("Field3 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(4); //Field4
	if(OK == rlt)
		{
			ptrdecoder->Field4 = curpos;
			curpos += (ptrdecoder->Field4_l = 6);
		}
	else if(FAIL == rlt)
		{
			printf("Field4 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}
	
	rlt = TEST(5); //Field5
	if(OK == rlt)
		{
			ptrdecoder->Field5 = curpos;
			curpos += (ptrdecoder->Field5_l = 16);
		}
	else if(FAIL == rlt)
		{
			printf("Field5 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknow error.\n");
			return FAIL;
		}

	rlt = TEST(6); //Field6
	if(OK == rlt)
		{
			ptrdecoder->Field6 = curpos;
			curpos += (ptrdecoder->Field6_l = 4);
		}
	else if(FAIL == rlt)
		{
			printf("Field6 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(7) || TEST(8) || TEST(9) ||TEST(10)) //Field7,8,9,10 undefined.
		{
			printf("undefined field 7,8,9,10 found.\n");
			return FAIL;
		}
	
	rlt = TEST(11); //Field11
	if(OK == rlt)
		{
			ptrdecoder->Field11 = curpos;
			curpos += (ptrdecoder->Field11_l = 3);
		}
	else if(FAIL == rlt)
		{
			printf("Field11 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(12); //Field12
	if(OK == rlt)
		{
			ptrdecoder->Field12 = curpos;
			curpos += (ptrdecoder->Field12_l = 3);
		}
	else if(FAIL == rlt)
		{
			printf("Field12 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(13); //Field13
	if(OK == rlt)
		{
			ptrdecoder->Field13 = curpos;
			curpos += (ptrdecoder->Field13_l = 2);
		}
	else if(FAIL == rlt)
		{
			printf("Field13 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(14); //Field14
	if(OK == rlt)
		{
			ptrdecoder->Field14 = curpos;
			curpos += (ptrdecoder->Field14_l = 2);
		}
	else if(FAIL ==rlt )
		{
			printf("Field14 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(15); //Field15
	if(OK == rlt)
		{
			ptrdecoder->Field15 = curpos;
			curpos += (ptrdecoder->Field15_l = 2);
		}
	else if(FAIL == rlt)
		{
			printf("Field15 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(16) || TEST(17) || TEST(18) || TEST(19) || TEST(20) || TEST(21)) //Field16,17,18,19,20,21 are not defined.
		{
			printf("undefined field16,17,18,19,20,21 found.\n");
			return FAIL;
		}

	rlt = TEST(22);  //Field22
	if(OK == rlt)
		{
			ptrdecoder->Field22 = curpos;
			curpos += (ptrdecoder->Field22_l = 2);
		}
	else if(FAIL == rlt)
		{
			printf("Field22 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(23); //Field23
	if(OK == rlt)
		{
			ptrdecoder->Field23 = curpos;
			curpos += (ptrdecoder->Field23_l = 2);
		}
	else if(FAIL == rlt)
		{
			printf("Field23 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(24)) //Field24 is not defined.
		{
			printf("undefined field24 found.\n");
			return FAIL;
		}

	rlt = TEST(25);  //Field25
	if(OK == rlt)
		{
			ptrdecoder->Field25 = curpos;
			curpos += (ptrdecoder->Field25 = 1);
		}
	else if(FAIL == rlt)
		{
			printf("Field25 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(26); //Field26
	if(OK == rlt)
		{
			ptrdecoder->Field26 = curpos;
			curpos += (ptrdecoder->Field26_l = 1);
		}
	else if(FAIL == rlt)
		{
			printf("Field26 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(27) || TEST(28) || TEST(29) || TEST(30) || TEST(31)) //Field27,28,29,30,31 are not defined.
		{
			printf("undefined field27,28,29,30,31 found.\n");
			return FAIL;
		}

	rlt = TEST(32);  //Field32
	if(OK == rlt)
		{
			ptrdecoder->Field32 = curpos;
			ptrdecoder->Field32_l = (1 + (BCDCalInMsgLenField(src + curpos, 1, RIGHT))/2);
			curpos += (ptrdecoder->Field32_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field32 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(33) || TEST(34) || TEST(35)) //Field33,34,35 are not defined.
		{
			printf("undefined field33,34,35 found.\n");
			return FAIL;
		}

	rlt = TEST(36); //Field36
	if(OK == rlt)
		{
			ptrdecoder->Field36 = curpos;
			ptrdecoder->Field36_l = (1 + (BCDCalInMsgLenField(src + curpos, 2, RIGHT))/2);
			curpos += (ptrdecoder->Field36_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field 36 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(37); //Field37
	if(OK == rlt)
		{
			ptrdecoder->Field37 = curpos;
			curpos += (ptrdecoder->Field37_l = 12);
		}
	else if(FAIL == rlt)
		{
			printf("Field37 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(38); //Field 38
	if(OK == rlt)
		{
			ptrdecoder->Field38 = curpos;
			curpos += (ptrdecoder->Field38_l = 6);
		}
	else if(FAIL == rlt)
		{
			printf("Field38 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(39); //Field 39
	if(OK == rlt)
		{
			ptrdecoder->Field39 = curpos;
			curpos += (ptrdecoder->Field39_l = 2);
		}
	else if(FAIL == rlt)
		{
			printf("Field39 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(40)) //Field40 is not defined.
		{
			printf("undefined field40 found.\n");
			return FAIL;
		}

	rlt = TEST(41); //Field 41
	if(OK == rlt)
		{
			ptrdecoder->Field41 = curpos;
			curpos += (ptrdecoder->Field41 = 8);
		}
	else if(FAIL == rlt)
		{
			printf("Field41 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(42); //Field42
	if(OK == rlt)
		{
			ptrdecoder->Field42 = curpos;
			curpos += (ptrdecoder->Field42_l = 15);
		}
	else if(FAIL == rlt)
		{
			printf("Field41 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(43)) //Field43 is not defined.
		{
			printf("undefined field 43 found.\n");
			return FAIL;
		}

	rlt = TEST(44); //Field 44
	if(OK == rlt)
		{
			ptrdecoder->Field44 = curpos;
			ptrdecoder->Field44_l = (1 + ((BCDCalInMsgLenField(src + curpos, 1, RIGHT))/2));
			curpos += ptrdecoder->Field44_l;
		}
	else if(FAIL == rlt)
		{
			printf("Field44 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(45)) // Field45 is not defined.
		{
			printf("undefined field 45 found.\n");
			return FAIL;
		}
	
	rlt = TEST(46); // Field 46 XGD 
	if(OK == rlt)
		{
			ptrdecoder->Field46 = curpos;
			ptrdecoder->Field46_l = (2 + ((BCDCalInMsgLenField(src + curpos, 2, RIGHT))/2));
			curpos += (ptrdecoder->Field46_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field46 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(47); // Field47
	if(OK == rlt)
		{
			ptrdecoder->Field47 = curpos;
			//ptrdecoder->Field47_l = (2 + ((BCDCalInMsgLenField(src + curpos, 2, RIGHT))/2));
			ptrdecoder->Field47_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field47_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field47 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(48); //Field 48
	if(OK == rlt)
		{
			ptrdecoder->Field48 = curpos;
			ptrdecoder->Field48_l = (2 + (BCDCalInMsgLenField(src + curpos, 2, RIGHT)));
			curpos += (ptrdecoder->Field48_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field48 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(49); //Field49
	if(OK == rlt)
		{
			ptrdecoder->Field49 = curpos;
			curpos += (ptrdecoder->Field49_l = 3);
		}
	else if(FAIL == rlt)
		{
			printf("Field49 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(50); //Field 50
	if(OK == rlt)
		{
			ptrdecoder->Field50 = curpos;
			ptrdecoder->Field50_l = (2 + (BCDCal(src + curpos, 2, RIGHT)));
			curpos += (ptrdecoder->Field50_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field50 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(51)) // Field 51 is not defined.
		{
			printf("undefined field 51 found.\n");
			return FAIL;
		}

	rlt = TEST(52); //Field52
	if(OK == rlt)
		{
			ptrdecoder->Field52 = curpos;
			curpos += (ptrdecoder->Field52_l = 8);
		}
	else if(FAIL == rlt)
		{
			printf("Field52 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(53); //Field53
	if(OK == rlt)
		{
			ptrdecoder->Field53 = curpos;
			curpos += (ptrdecoder->Field53 = 8);
		}
	else if(FAIL == rlt)
		{
			printf("Field53 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(54); //Field 54
	if(OK == rlt)
		{
			ptrdecoder->Field54 = curpos;
			ptrdecoder->Field54_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field54_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field54 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(55); //Field 55
	if(OK == rlt)
		{
			ptrdecoder->Field55 = curpos;
			ptrdecoder->Field55_l = (2 + ((BCDCalInMsgLenField(src + curpos, 2, RIGHT)) / 2));
			curpos += (ptrdecoder->Field55_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field55 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(56); //Field 56
	if(OK == rlt)
		{
			ptrdecoder->Field56 = curpos;
			ptrdecoder->Field56_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field56_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field56 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	if(TEST(57)) // Field 57 is not defined.
		{
			printf("undefined field 57 found.\n");
			return FAIL;
		}

	rlt = TEST(58); //Field 58
	if(OK == rlt)
		{
			ptrdecoder->Field58 = curpos;
			ptrdecoder->Field58_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field58_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field58 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(59); //Field 59
	if(OK == rlt)
		{
			ptrdecoder->Field59 = curpos;
			ptrdecoder->Field59_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field59_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field59 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(60); //Field 60
	if(OK == rlt)
		{
			ptrdecoder->Field60 = curpos;
			ptrdecoder->Field60_l = (2 + ((BCDCalInMsgLenField(src + curpos, 2, RIGHT)) / 2));
			curpos += (ptrdecoder->Field60_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field60 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(61); //Field 61
	if(OK == rlt)
		{
			ptrdecoder->Field61 = curpos;
			ptrdecoder->Field61_l = (2 + ((BCDCalInMsgLenField(src + curpos, 2, RIGHT)) / 2));
			curpos += (ptrdecoder->Field61_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field 61 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(62); //Field 62
	if(OK == rlt)
		{
			ptrdecoder->Field62 = curpos;
			ptrdecoder->Field62_l = (2 + BCDCal(src + curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field62_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field 62 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(63); //Field 63
	if(OK == rlt)
		{
			ptrdecoder->Field63 = curpos;
			ptrdecoder->Field63_l = (2 + BCDCal(src +curpos, 2, RIGHT));
			curpos += (ptrdecoder->Field63_l);
		}
	else if(FAIL == rlt)
		{
			printf("Field 63 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	rlt = TEST(64);
	if(OK == rlt)
		{
			ptrdecoder->Field64 = curpos;
			curpos += (ptrdecoder->Field64_l = 8);
		}
	else if(FAIL == rlt)
		{
			printf("Field 64 not exist.\n");
		}
	else
		{
			printf("bitmap decode unknown error.\n");
			return FAIL;
		}

	
#undef TEST

	free(BitMap);
}





