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