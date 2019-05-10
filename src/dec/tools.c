#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "8583.h"

ConsoleBuf consolebuffer = {0};

int initConsoleBuf(){
    consolebuffer.w = consolebuffer.buf;
    for(int i=0; i<CONSOLEBUFSIZE; i++)
        consolebuffer.buf[i] = 0;
    return 1;
}

int clearConsoleBuf(){
    consolebuffer.w = consolebuffer.buf;
    for(int i=0; i<CONSOLEBUFSIZE; i++)
        consolebuffer.buf[i] = 0;
    return 1;
}

int printConsole(s8* str)
{
    if(strlen(str) >= CONSOLEBUFSIZE)
        return -1;
    if((strlen(consolebuffer.buf) + strlen(str)) < CONSOLEBUFSIZE){
        strcat(consolebuffer.buf, str);
        return 1;
    }else{
        clearConsoleBuf();
        strcat(consolebuffer.buf, str);
        return 0;
    }
}

typedef union{
		u32 a;
		u8 b;
		u8 c[3];
}U4_BIGEDIAN;
typedef union{
		u32 a;
		u8 b[3];
		u8 c;
}U4_LITEDIAN;
u32 calLenField(const u8* src, u32 len, DIGITENCODEFORMAT flag)
{
    u32 val = 0;
    switch(flag){
        case NOFORMAT:
        case NOFORMAT_BIGEDIAN:     			
			U4_BIGEDIAN u4 = {0};
            switch(len){
				case 0: return 0;
				case 1: return (u32)(*src);
				case 2: 
					u16 tmpu16 = 0;
					toLittleEndian((u16*)src, sizeof(u16), &tmpu16)
					return (u32)tmpu16;
				case 3:
					u4.c[0] = *src++;
					u4.c[1] = *src++;
					u4.c[2] = *src;
					toLittleEndian(&(u4.a), sizeof(u4), &(u4.a));
					return u4.a;
				case 4: 
					u32 tmpu32 = 0;
					toLittleEndian((u32*)src, sizeof(u32), &tmpu32);
					return tmpu32;
				default: 
					printConsole("calLenField failed");
					return 0;
            	}
        case NOFORMAT_LITEDIAN:            
			U4_LITEDIAN u4 = {0};
            switch(len){
				case 0: return 0;
				case 1: return (u32)(*src);
				case 2: 
					return (u32)*(u16*)src;
				case 3:
					u4.b[0] = *src++;
					u4.b[1] = *src++;
					u4.b[2] = *src;
					return u4.a;
				case 4: return *(u32*)src;
				default: 
					printConsole("calLenField failed");
					return 0;
            	}
        case BCD:
        case BCDRALIGN:
			s8 bcdEncoded[2 * len + 1];
			for(size_t i=0; i < 2*len + 1; i++)
				bcdEncoded[i] = 0;
			if(BCDDecode(src, len, bcdEncoded) != 1)
				return 0;
			s8 *ptr = bcdEncoded;
			int i = strlen(bcdEncoded);
			while(*ptr){
				if(*ptr != 0x30){
					u32 tmpval = *ptr - 0x30;
					for(int k=0; k<i-1; k++)
						tmpval *= 10;
					val += tmpval;
				}
				i--;
				ptr++;
			}
			return val;			
        case BCDLALIGN:
            printConsole("BCD digit left align is not supported.\n");
            return 0;
        case ASCII:
            if(len < 1){
                printConsole("illegal ASCII digit.\n");
                return 0;
            }
            u32 valtmp = 0;
            for(int i=0; i<len; i++)
            {
                valtmp = *src++;
                if(!isdigit(valtmp)){
                    printConsole("illegal ASCII digit.\n");
                    return 0;
                }
                valtmp -= 0x30;
                for(int j=1; j<(len - i); j++){
                    valtmp *= 10;
                }
                val += valtmp;
            }            
            return val;
        default:
            printConsole("illegal digit encode format specified.\n");
            return 0;
    }
}
u32 calLenFieldOfCompressedAlign(const u8* src, u32 len, DIGITENCODEFORMAT flag){
    u32 rlt = calLenField(src, len, flag);
    if(!rlt)
        return rlt;
    if((rlt % 2) == 1){
        return (rlt + 1) / 2;
    }else{
        return rlt / 2;
    }
}

int HexcharStringRevert(const s8* src, u8* dst, u32 len)
{
    if(strlen(src) == 0)
    {
        printConsole("illegal hex string.\n");
        return -1;
    }
    if(strlen(src) != len*2){
        printConsole("illegal hex string.\n");
        return -1;
    }
    for(int i=0; i<len; i++)
    {
        if(!isxdigit(*(src + 2*i))){
            s8 tmpmsg[TMPMSGSIZE] = {0};
            sprintf(tmpmsg, "illegal hex char %c at %d.\n", *(src + 2*i), 2*i);
            printConsole(tmpmsg);
            return -1;
        }
        if(!isxdigit(*(src + 2*i + 1))){
            s8 tmpmsg[TMPMSGSIZE] = {0};
            sprintf(tmpmsg, "illegal hex char %c at %d.\n", *(src + 2*i + 1), 2*i + 1);
            printConsole(tmpmsg);
            return -1;
        }
        u8 valh, vall;
        switch(*(src + 2*i)){
            case '0': 
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                valh = *(src + 2*i) - 0x30; break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                valh = *(src + 2*i) - 0x57; break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                valh = *(src + 2*i) - 0x37; break;
            default:
                printf("unkonwn error.\n");
                return -1;
        }
        switch(*(src + 2*i +1)){
            case '0': 
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                vall = *(src + 2*i +1) - 0x30; break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                vall = *(src + 2*i +1) - 0x57; break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                vall = *(src + 2*i +1) - 0x37; break;
            default:
                printf("unkonwn error.\n");
                return -1;
        }
        *(dst + i) = (valh << 4) | vall;
    }
    return 1;
}

void printMem(const u8* src, u32 len)
{
    int i;
    for(i=0; i< len; i++){
        printf("%.2x ", *src++);
        if((i+1) % 16 == 0)
            printf("\n");
    }
    if((i+1) % 16 != 0)
        printf("\n");
}
void printMemS(const u8* src, u32 len, s8* dst)
{
    if( len > (MAXMSGSIZE / 2)){
        printConsole("dst too short.\n");
        memset(dst, 0, strlen(dst));
        return;
    }
    int i;
    for(i=0; i< len; i++){
        sprintf(dst,"%.2x", *src++);
        dst += 2;
    }
}

/*-----------------------------
Test the bitmap(*src), if the index sequnce of bit in memory *src matched 1, test sucess.
else test fail.
-----------------------------*/
int BitMaptest(const int index, const u8 *src, u32 len)
{
	if((index < 1) || (index > ((8*len) +1)))
		{
            s8 tmpmsg[TMPMSGSIZE] = {0};
			sprintf(tmpmsg, "ivalid index %d or bitmap length %u \n", index, len);
            printConsole(tmpmsg);
			return FATAL_ERROR;
		}	
	u8 *Mask = (u8*)malloc(len);
	memset(Mask, 0, len);
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
		default:
            printConsole("bitmap index error \n"); 
            return FATAL_ERROR;
	}
	u32 i = ((index -1) / 8);	
	*(Mask + i) = chr;	
    u8 tmp = 0;
	for(u32 j = 0; j < len; j++)
		{
            tmp = (*(Mask + j)) & (*src++);
			if(tmp)
				return OK;
		}	
	free(Mask);	
	return FAIL;
}
u8* testTPDU(const s8 *tpdu, const u8 *src, u32 len, u32 *dstLen)
{
    if(10 != strlen(tpdu))
        return NULL;
    u8 *tmp = (u8*)malloc(5);
    if(!(1 == HexcharStringRevert(tpdu, tmp, 5)))
        return NULL;
    u64 l,r;
    TPDUUnion ul = {0};
    ul.tpdu = *(TPDU*)tmp;
    l = ul.val;
    for(int i = 0; i < (len - 5); i++, src++){
        TPDUUnion ur = {0};
        ur.tpdu = *(TPDU*)src;
        r = ur.val;
        if(!(l ^ r)){
            *dstLen = 2 + ((*(src - 2) & 0x000000FF) << 8) + ((*(src - 1) & 0x000000FF)); //big edian , little edian
            if((i + *dstLen - 2) > len)
                return NULL;
            if(i > 1)
                return src - 2;
        }
    }
    return NULL;
}

void* toLittleEndian(const void* src, size_t len, void* dst){
	if(src != dst)
		for(size_t i = 0; i < len; i++){		
			*(char*)(dst + len - 1 - i) = *(char*)(src + i);
		}
	else{
		for(size_t i = 0; i < len / 2; i++){
			char tmp = *(char*)(dst + i);
			*(char*)(dst + i) = *(char*)(dst + len - 1 - i);
			*(char*)(dst + len - 1 - i) = tmp;
		}
	}		
	return dst;
}
void* toBigEndian(const void* src, size_t len, void* dst){
	return toLittleEndian(src, len, dst);
}
int BCDEncode(const char* src, size_t len, void* dst, BCDALIGN align){
	size_t srclen = strlen(src);
	if(srclen <= 2 * len){
		u8 newsrc[2 * len + 1];
		u8 *newsrcptr = newsrc;
		for(size_t i=0; i<2*len; i++)
			newsrc[i] = 0x30;
		newsrc[2*len] = 0;
		switch(align){
			case BCDRIGHTALIGN:
				memcpy(newsrc + (2*len - srclen), (u8*)src, srclen);				
				while(*newsrcptr){
					if(isdigit(*newsrcptr)){
						u8 h4 = (*newsrcptr++ - 0x30) << 4;
						u8 l4 = *newsrcptr++ - 0x30;
						*(u8*)dst++ = h4 + l4;						
					}
					else
						return -1;
				}
				return 1;
			case BCDLEFTALIGIN:
				memcpy(newsrc, (u8*)src, srclen);
				while(*newsrcptr){
					if(isdigit(*newsrcptr)){
						u8 h4 = (*newsrcptr++ - 0x30) << 4;
						u8 l4 = *newsrcptr++ - 0x30;
						*(u8*)dst++ = h4 + l4;						
					}
					else
						return -1;
				}
				return 1;
			default: return -1;
		}		
	}
	else
		return -1;
}
int BCDDecode(const void* src, size_t len, char* dst){
	for(size_t i = 0; i < len; i++){
		if(((*(u8*)src & 0xF0) >> 4) < 0x0A)
			*(dst + 2*i) = ((*(u8*)src & 0xF0) >> 4) + 0x30;
		else
			return -1;
		if((*(u8*)src & 0x0F) < 0x0A)
			*(dst + 2*i + 1) = (*(u8*)src & 0x0F) + 0x30;
		else
			return -1;
		(u8*)src++;
	}
	*(u8*)(dst + 2 * len) = 0x00; 
	return 1;
}