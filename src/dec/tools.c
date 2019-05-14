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

u32 calLenField(const u8* src, u32 len, DIGITENCODEFORMAT flag)
{
    u32 val = 0;
    switch(flag){
        case NOFORMAT:
        case NOFORMAT_BIGEDIAN:{
			union U4{
                u32 a;
                u8 b[4];
            }u4 = {0};
            switch(len){
				case 0: return 0;
				case 1: return (u32)(*src);
				case 2:{
					u16 tmpu16 = 0;
					toLittleEndian((u16*)src, sizeof(u16), &tmpu16);
					return (u32)tmpu16;
                }
				case 3:{
					u4.b[1] = *src++;
					u4.b[2] = *src++;
					u4.b[3] = *src;
					toLittleEndian(&(u4.a), sizeof(u4), &(u4.a));
					return u4.a;
                }
				case 4:{
					u32 tmpu32 = 0;
					toLittleEndian((u32*)src, sizeof(u32), &tmpu32);
					return tmpu32;
                }
				default: 
					printConsole("calLenField failed");
					return 0;
            }
            break;
        }
        case NOFORMAT_LITEDIAN:{
			union U4{
                u32 a;
                u8 b[4];
            }u4;
            switch(len){
				case 0: return 0;
				case 1: return (u32)(*src);
				case 2: return (u32)*(u16*)src;
				case 3:
					u4.b[1] = *src++;
					u4.b[2] = *src++;
					u4.b[3] = *src;
					return u4.a;
				case 4: return *(u32*)src;
				default: 
					printConsole("calLenField failed");
					return 0;
            }
            break;
        }
        case BCD:
        case BCDRALIGN:{
			s8 bcdEncoded[2 * len + 1];
			memset(bcdEncoded, 0, 2 * len +1);
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
				++ptr;
			}
			return val;
        }
        case BCDLALIGN:
            printConsole("BCD digit left align is not supported.\n");
            return 0;
        case ASCII:{
            if(len < 1){
                printConsole("illegal ASCII digit.\n");
                return 0;
            }
            u32 valtmp = 0;
            const u8* p = src;
            u32 i = len;
            while(p != (src + len)){
                if(!isdigit(*p)){
                    printConsole("illegal ASCII digit.\n");
                    return 0;
                }
                valtmp -= 0x30;
                for(int j=1; j<(i - 1); j++){
                    valtmp *= 10;
                }
                val += valtmp;
                --i;
                ++p;
            }
            return val;
        }
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
    const s8* p = src;
    while(*p){
        if(!isxdigit(*p)){
            s8 tmpmsg[TMPMSGSIZE] = {0};
            sprintf(tmpmsg, "illegal hex char %c at %I64d.\n", *p, p - src);
            printConsole(tmpmsg);
            return -1;
        }
        u8 valh = xtoi(*p++) << 4;
        u8 vall = xtoi(*p++);
        *dst++ = valh | vall;
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
	if((index < 1) || (index > (8*len)))
		{
            s8 tmpmsg[TMPMSGSIZE] = {0};
			sprintf(tmpmsg, "ivalid index %d or bitmap length %u \n", index, len);
            printConsole(tmpmsg);
			return FATAL_ERROR;
		}
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
    if(chr & *(src + i))
        {return OK;}
	return FAIL;
}
u8* testTPDU(CUSTOMERID cid, const s8 *tpdu, const u8 *src, u32 len, u32 *dstLen)
{
    if(10 != strlen(tpdu))
        return NULL;
    u8 tmp[5];
    if(!(1 == HexcharStringRevert(tpdu, tmp, 5)))
        return NULL;
    TPDUUnion utpdu = {0};
    memcpy(utpdu.tpdu, tmp, sizeof(utpdu.tpdu));
    for(const u8* p = src; p != (src + len -5); ++p){
        TPDUUnion ufind = {0};
        memcpy(ufind.tpdu, p, sizeof(ufind.tpdu));
        if(ufind.val == utpdu.val){
            u16 tmplen = 0;
            u32 rightVCheck = 0;
            toLittleEndian(p-2, 2, &tmplen);
            switch(cid){
                case CUSTOMER_CUP:
                case CUSTOMER_JL:
                    *dstLen = 2 + tmplen;
                    rightVCheck = p + tmplen -src;
                    break;
                case CUSTOMER_YS:
                    *dstLen = tmplen;
                    rightVCheck = p + tmplen -src - 2;
                    break;
                default: return NULL;
            }
            //*dstLen = 2 + tmplen; //big edian , little edian
            //if((p + tmplen -src) > len) //check right side violation
            if(rightVCheck > len)
                return NULL;
            if((p - src) >= 2) //check left side violation
                return p - 2;
        }
    }
    return NULL;
}
int BCDEncode(const char* src, size_t len, void* dst, DIGITENCODEFORMAT align){
	size_t srclen = strlen(src);
	if(srclen <= 2 * len){
		u8 newsrc[2 * len + 1];		
		for(u8* p = newsrc; p != newsrc + (2 * len); ++p){
            *p = 0x30;
        }
		newsrc[2*len] = 0;
        u8 *newsrcptr = newsrc;
		switch(align){
			case BCDRALIGN:
				memcpy(newsrc + (2*len - srclen), (u8*)src, srclen);
				while(*newsrcptr){
					if(isdigit(*newsrcptr)){
						u8 h4 = (*newsrcptr++ - 0x30) << 4;
						u8 l4 = *newsrcptr++ - 0x30;
						*(u8*)dst++ = h4 | l4;
					}
					else
						return -1;
				}
				return 1;
			case BCDLALIGN:
				memcpy(newsrc, (u8*)src, srclen);
				while(*newsrcptr){
					if(isdigit(*newsrcptr)){
						u8 h4 = (*newsrcptr++ - 0x30) << 4;
						u8 l4 = *newsrcptr++ - 0x30;
						*(u8*)dst++ = h4 | l4;
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
	for(u8* p = (u8*)src; p != (u8*)src + len; p++){
		if(((*p & 0xF0) >> 4) < 0x0A)
			*dst++ = ((*p & 0xF0) >> 4) + 0x30;
		else
			return -1;
		if((*p & 0x0F) < 0x0A)
			*dst++ = (*p & 0x0F) + 0x30;
		else
			return -1;
	}
	*dst = 0x00; 
	return 1;
}