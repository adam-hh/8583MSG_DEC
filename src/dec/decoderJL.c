#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "8583.h"
	
/*
 *description:this macro used to shield unwanted parameters for msgScan func
 *x is the field number and bitmap index
 *y is length
 *z is length field flag
 */
#define TEST(x, y, z) \
if(FATAL_ERROR == msgScan(&(dst->Field##x), "Field" #x, &(dst->Field##x##_l), dst->Field1, dst->Field1_l, x, &curpos, y, z)) \
	return FATAL_ERROR; \
if((curpos - src) > len) { \
	printConsole("fatal error: memory violation detected when process decoding.\n"); \
	return FATAL_ERROR; \
}
int DecodeJLMsg(const u8 *src, u32 len, void* dest)
{
    if(len < 16){
        printConsole("message length too short.\n");
        return FAIL;
    }
	
	const u8* curpos = src; //cursor
	MsgJL* dst = (MsgJL*)dest; //cast
	memset(dst, 0, sizeof(MsgJL)); //set 0
    s8 tmpmsg[MAXMSGSIZE] = {0}; //used to format a field data to hex string
	s8 tmp[MAXMSGSIZE + TMPMSGSIZE] = {0}; //used to format a whole message string

    memcpy(dst->MsgLen, curpos, sizeof(dst->MsgLen)); 
    curpos += sizeof(dst->MsgLen);
	printMemS(dst->MsgLen, sizeof(dst->MsgLen), tmpmsg);
	sprintf(tmp, "MsgLen:%s\n", tmpmsg);
	printConsole(tmp);

    memcpy(dst->MsgTPDU, curpos, sizeof(dst->MsgTPDU));
    curpos += sizeof(dst->MsgTPDU);
	printMemS(dst->MsgTPDU, sizeof(dst->MsgTPDU), tmpmsg);
	sprintf(tmp, "MsgTPDU:%s\n", tmpmsg);
	printConsole(tmp);;

    memcpy(dst->MsgHead, curpos, sizeof(dst->MsgHead));
    curpos += sizeof(dst->MsgHead);
	printMemS(dst->MsgHead, sizeof(dst->MsgHead), tmpmsg);
	sprintf(tmp, "MsgHead:%s\n", tmpmsg);
	printConsole(tmp);

    memcpy(dst->Field0, curpos, sizeof(dst->Field0));
    curpos += sizeof(dst->Field0);
	printMemS(dst->Field0, sizeof(dst->Field0), tmpmsg);
	sprintf(tmp, "Field0:%s\n", tmpmsg);
	printConsole(tmp);
	
	dst->Field1 = curpos; //Field1 is bitmap
    if(*curpos > 0x80){
    	dst->Field1_l = 16;
    	curpos += 16;
		printMemS(dst->Field1, dst->Field1_l, tmpmsg);
		sprintf(tmp, "Field1:%s\n", tmpmsg);
		printConsole(tmp);
    }else{
        dst->Field1_l = 8;
        curpos += 8;
		printMemS(dst->Field1, dst->Field1_l, tmpmsg);
		sprintf(tmp, "Field1:%s\n", tmpmsg);
		printConsole(tmp);
    }

	TEST(2, 1, L_BCD_COMPRESS_ALIGN)
	TEST(3, 3, L_FIXED)
	TEST(4, 6, L_FIXED)
    TEST(5, 16, L_FIXED)
	TEST(6, 6, L_FIXED)
	TEST(7, 0, L_UNDEFINE)
	TEST(8, 0, L_UNDEFINE)
	TEST(9, 0, L_UNDEFINE)
	TEST(10, 0, L_UNDEFINE)
	TEST(11, 3, L_FIXED)
	TEST(12, 3, L_FIXED)
	TEST(13, 2, L_FIXED)
	TEST(14, 2, L_FIXED)
	TEST(15, 2, L_FIXED)
	TEST(16, 0, L_UNDEFINE)
	TEST(17, 0, L_UNDEFINE)
	TEST(18, 0, L_UNDEFINE)
	TEST(19, 0, L_UNDEFINE)
	TEST(20, 0, L_UNDEFINE)
	TEST(21, 0, L_UNDEFINE)
	TEST(22, 2, L_FIXED)
	TEST(23, 2, L_FIXED)
	TEST(24, 0, L_UNDEFINE)
	TEST(25, 1, L_FIXED)
	TEST(26, 1, L_FIXED)
	TEST(27, 0, L_UNDEFINE)
	TEST(28, 0, L_UNDEFINE)
	TEST(29, 0, L_UNDEFINE)
	TEST(30, 0, L_UNDEFINE)
	TEST(31, 0, L_UNDEFINE)
	TEST(32, 1, L_BCD_COMPRESS_ALIGN)
	TEST(33, 0, L_UNDEFINE)
	TEST(34, 0, L_UNDEFINE)
	TEST(35, 1, L_BCD_COMPRESS_ALIGN)
	TEST(36, 2, L_BCD_COMPRESS_ALIGN)
	TEST(37, 12, L_FIXED)
	TEST(38, 6, L_FIXED)
	TEST(39, 2, L_FIXED)
	TEST(40, 0, L_UNDEFINE)
	TEST(41, 8, L_FIXED)
	TEST(42, 15, L_FIXED)
	TEST(43, 0, L_UNDEFINE)
	TEST(44, 1, L_BCD)
	TEST(45, 0, L_UNDEFINE)
	TEST(46, 2, L_BCD)
	TEST(47, 2, L_BCD)
	TEST(48, 2, L_BCD_COMPRESS_ALIGN)
	TEST(49, 3, L_FIXED)
	TEST(50, 2, L_BCD)
	TEST(51, 0, L_UNDEFINE)
	TEST(52, 8, L_FIXED)
	TEST(53, 8, L_FIXED)
	TEST(54, 2, L_BCD)
	TEST(55, 2, L_BCD)
	TEST(56, 2, L_BCD)
	TEST(57, 0, L_UNDEFINE)
	TEST(58, 2, L_BCD)
	TEST(59, 2, L_BCD)
	TEST(60, 2, L_BCD_COMPRESS_ALIGN)
	TEST(61, 2, L_BCD_COMPRESS_ALIGN)
	TEST(62, 2, L_BCD)
	TEST(63, 2, L_BCD)
	TEST(64, 8, L_FIXED)
    return OK;
}
#undef TEST