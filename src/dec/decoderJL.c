#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "8583.h"

int DecodeJLMsg(const u8 *src, u32 len, void* dest)
{
    if(len < 16){
        printConsole("message length too short.\n");
        return FAIL;
    }
#define MEMVIOLATIONCHECK if((curpos - src) > len) {printConsole("fatal error: memory violation detected when process decoding.\n");return FATAL_ERROR;}
#define TEST(x) BitMaptest(x, dst->Field1, dst->Field1_l)
#define PRINT(x) printConsole(#x); printConsole(":"); printMemS(dst->x, dst->x##_l, tmpmsg); printConsole(tmpmsg); printConsole("\n")
#define PRINT_F(x) printConsole(#x); printConsole(" not exist.\n")
#define PRINT_E(x) printConsole("fatal error(unknow) when processing "); printConsole(#x); printConsole("\n"); return FATAL_ERROR;
#define PRINT_NDEF(x) printConsole("fatal error: undefined "); printConsole(#x); printConsole(" found.\n"); return FAIL;
	const u8* curpos = src;
	int rlt = FAIL;
	MsgJL* dst = (MsgJL*)dest;
	memset(dst, 0, sizeof(MsgJL));
    s8 tmpmsg[MAXMSGSIZE] = {0};

    memcpy(dst->MsgLen, curpos, sizeof(dst->MsgLen)); 
    curpos += sizeof(dst->MsgLen);MEMVIOLATIONCHECK
	printConsole("MsgLen:");
	printMemS(dst->MsgLen, sizeof(dst->MsgLen), tmpmsg);
	printConsole(tmpmsg);
	printConsole("\n");

    memcpy(dst->MsgTPDU, curpos, sizeof(dst->MsgTPDU));
    curpos += sizeof(dst->MsgTPDU);MEMVIOLATIONCHECK
	printConsole("MsgTPDU:");
	printMemS(dst->MsgTPDU, sizeof(dst->MsgTPDU), tmpmsg);
	printConsole(tmpmsg);
	printConsole("\n");

    memcpy(dst->MsgHead, curpos, sizeof(dst->MsgHead));
    curpos += sizeof(dst->MsgHead);MEMVIOLATIONCHECK
	printConsole("Msghead:");
	printMemS(dst->MsgHead, sizeof(dst->MsgHead), tmpmsg);
	printConsole(tmpmsg);
	printConsole("\n");

    memcpy(dst->Field0, curpos, sizeof(dst->Field0));
    curpos += sizeof(dst->Field0);MEMVIOLATIONCHECK
	printConsole("Field0:");
	printMemS(dst->Field0, sizeof(dst->Field0), tmpmsg);
	printConsole(tmpmsg);
	printConsole("\n");
	
	dst->Field1 = curpos;
    if(*curpos > 0x80){
    	dst->Field1_l = 16;
    	curpos += 16;MEMVIOLATIONCHECK
		PRINT(Field1);
    }else{
        dst->Field1_l = 8;
        curpos += 8;MEMVIOLATIONCHECK
		PRINT(Field1);
    }

	rlt = TEST(2);//Field2
	if(OK == rlt){
		dst->Field2 = curpos;
		dst->Field2_l = 1 + calLenFieldOfCompressedAlign(curpos, 1, BCD);
		curpos += (dst->Field2_l);MEMVIOLATIONCHECK
		PRINT(Field2);
	}else if(FAIL == rlt){
		PRINT_F(Field2);
	}else{
		PRINT_E(Field2);
	}

    rlt = TEST(3);//Field3
	if(OK == rlt){
		dst->Field3 = curpos;
		curpos += (dst->Field3_l = 3);MEMVIOLATIONCHECK
		PRINT(Field3);
	}else if(FAIL == rlt){
		PRINT_F(Field3);
	}else{
		PRINT_E(Field3);
	}
    
    rlt = TEST(4); //Field4
	if(OK == rlt){
		dst->Field4 = curpos;
		curpos += (dst->Field4_l = 6);MEMVIOLATIONCHECK
		PRINT(Field4);
	}else if(FAIL == rlt){
		PRINT_F(Field4);
	}else{
		PRINT_E(Field4);
	}
    
	rlt = TEST(5); //Field5
	if(OK == rlt){
		dst->Field5 = curpos;
		curpos += (dst->Field5_l = 16);MEMVIOLATIONCHECK
		PRINT(Field5);
	}
	else if(FAIL == rlt){
		PRINT_F(Field5);
	}else{
		PRINT_E(Field5);
	}

	rlt = TEST(6); //Field6
	if(OK == rlt){
		dst->Field6 = curpos;
		curpos += (dst->Field6_l = 4);MEMVIOLATIONCHECK
		PRINT(Field6);
	}else if(FAIL == rlt){
		PRINT_F(Field6);
	}else{
		PRINT_E(Field6);
	}

	if(TEST(7) || TEST(8) || TEST(9) ||TEST(10)){ //Field7,8,9,10 undefined.
		PRINT_NDEF(Field7_8_9_10);
	}
	
	rlt = TEST(11); //Field11
	if(OK == rlt){
		dst->Field11 = curpos;
		curpos += (dst->Field11_l = 3);MEMVIOLATIONCHECK
		PRINT(Field11);
	}else if(FAIL == rlt){
		PRINT_F(Field11);
	}else{
		PRINT_E(Field11);
	}

	rlt = TEST(12); //Field12
	if(OK == rlt){
		dst->Field12 = curpos;
		curpos += (dst->Field12_l = 3);MEMVIOLATIONCHECK
		PRINT(Field12);
	}else if(FAIL == rlt){
		PRINT_F(Field12);
	}else{
		PRINT_E(Field12);
	}

	rlt = TEST(13); //Field13
	if(OK == rlt){
		dst->Field13 = curpos;
		curpos += (dst->Field13_l = 2);MEMVIOLATIONCHECK
		PRINT(Field13);
	}else if(FAIL == rlt){
		PRINT_F(Field13);
	}else{
		PRINT_E(Field13);
	}

	rlt = TEST(14); //Field14
	if(OK == rlt){
		dst->Field14 = curpos;
		curpos += (dst->Field14_l = 2);MEMVIOLATIONCHECK
		PRINT(Field14);
	}else if(FAIL ==rlt ){
		PRINT_F(Field14);
	}else{
		PRINT_E(Field14);
	}

	rlt = TEST(15); //Field15
	if(OK == rlt){
		dst->Field15 = curpos;
		curpos += (dst->Field15_l = 2);MEMVIOLATIONCHECK
		PRINT(Field15);
	}else if(FAIL == rlt){
		PRINT_F(Field15);
	}else{
		PRINT_E(Field15);
	}

	if(TEST(16) || TEST(17) || TEST(18) || TEST(19) || TEST(20) || TEST(21)){ //Field16,17,18,19,20,21 are not defined.
		PRINT_NDEF(Field16_17_18_19_20_21);
	}

	rlt = TEST(22);  //Field22
	if(OK == rlt){
		dst->Field22 = curpos;
		curpos += (dst->Field22_l = 2);MEMVIOLATIONCHECK
		PRINT(Field22);
	}else if(FAIL == rlt){
		PRINT_F(Field22);
	}else{
		PRINT_E(Field22);
	}

	rlt = TEST(23); //Field23
	if(OK == rlt){
		dst->Field23 = curpos;
		curpos += (dst->Field23_l = 2);MEMVIOLATIONCHECK
		PRINT(Field23);
	}else if(FAIL == rlt){
		PRINT_F(Field23);
	}else{
		PRINT_E(Field23);
	}

	if(TEST(24)){ //Field24 is not defined.
		PRINT_NDEF(Field24);
	}

	rlt = TEST(25);  //Field25
	if(OK == rlt){
		dst->Field25 = curpos;
		curpos += (dst->Field25_l = 1);MEMVIOLATIONCHECK
		PRINT(Field25);
	}else if(FAIL == rlt){
		PRINT_F(Field25);
	}else{
		PRINT_E(Field25);
	}

	rlt = TEST(26); //Field26
	if(OK == rlt){
		dst->Field26 = curpos;
		curpos += (dst->Field26_l = 1);MEMVIOLATIONCHECK
		PRINT(Field26);
	}else if(FAIL == rlt){
		PRINT_F(Field26);
	}else{
		PRINT_E(Field26);
	}

	if(TEST(27) || TEST(28) || TEST(29) || TEST(30) || TEST(31)){ //Field27,28,29,30,31 are not defined.
		PRINT_NDEF(Field27_28_29_30_31);
	}

	rlt = TEST(32);  //Field32
	if(OK == rlt){
		dst->Field32 = curpos;
		dst->Field32_l = 1 + calLenFieldOfCompressedAlign(curpos, 1, BCD);
		curpos += (dst->Field32_l);MEMVIOLATIONCHECK
		PRINT(Field32);
	}else if(FAIL == rlt){
		PRINT_F(Field32);
	}else{
		PRINT_E(Field32);
	}

	if(TEST(33) || TEST(34)){ //Field33,34 are not defined.
		PRINT_NDEF(Field33_34);
	}

	rlt = TEST(35); //Field35
	if(OK == rlt){
		dst->Field35 = curpos;
		dst->Field35_l = 1 +calLenFieldOfCompressedAlign(curpos, 1, BCD);
		curpos += (dst->Field35_l);MEMVIOLATIONCHECK
		PRINT(Field35);
	}else if(FAIL == rlt){
		PRINT_F(Field35);
	}else{
		PRINT_E(Field35);
	}

	rlt = TEST(36); //Field36
	if(OK == rlt){
		dst->Field36 = curpos;
		dst->Field36_l = 2 + calLenFieldOfCompressedAlign(curpos, 2, BCD);
		curpos += (dst->Field36_l);MEMVIOLATIONCHECK
		PRINT(Field36);
	}else if(FAIL == rlt){
		PRINT_F(Field36);
	}else{
		PRINT_E(Field36);
	}

	rlt = TEST(37); //Field37
	if(OK == rlt){
		dst->Field37 = curpos;
		curpos += (dst->Field37_l = 12);MEMVIOLATIONCHECK
		PRINT(Field37);
	}else if(FAIL == rlt){
		PRINT_F(Field37);
	}else{
		PRINT_E(Field37);
	}

	rlt = TEST(38); //Field 38
	if(OK == rlt){
		dst->Field38 = curpos;
		curpos += (dst->Field38_l = 6);MEMVIOLATIONCHECK
		PRINT(Field38);
	}else if(FAIL == rlt){
		PRINT_F(Field38);
	}else{
		PRINT_E(Field38);
	}

	rlt = TEST(39); //Field 39
	if(OK == rlt){
		dst->Field39 = curpos;
		curpos += (dst->Field39_l = 2);MEMVIOLATIONCHECK
		PRINT(Field39);
	}else if(FAIL == rlt){
		PRINT_F(Field39);
	}else{
		PRINT_E(Field39);
	}

	if(TEST(40)){ //Field40 is not defined.
		PRINT_NDEF(Field40);
	}

	rlt = TEST(41); //Field 41
	if(OK == rlt){
		dst->Field41 = curpos;
		curpos += (dst->Field41_l = 8);MEMVIOLATIONCHECK
		PRINT(Field41);
	}else if(FAIL == rlt){
		PRINT_F(Field41);
	}else{
		PRINT_E(Field41);
	}

	rlt = TEST(42); //Field42
	if(OK == rlt){
		dst->Field42 = curpos;
		curpos += (dst->Field42_l = 15);MEMVIOLATIONCHECK
		PRINT(Field42);
	}else if(FAIL == rlt){
		PRINT_F(Field42);
	}else{
		PRINT_E(Field42);
	}

	if(TEST(43)){ //Field43 is not defined.
		PRINT_NDEF(Field43);
	}

	rlt = TEST(44); //Field 44
	if(OK == rlt){
		dst->Field44 = curpos;
		dst->Field44_l = 1 + calLenField(curpos, 1, BCD);
		curpos += dst->Field44_l;MEMVIOLATIONCHECK
		PRINT(Field44);
	}else if(FAIL == rlt){
		PRINT_F(Field44);
	}else{
		PRINT_E(Field44);
	}

	if(TEST(45)){ // Field45 is not defined.
		PRINT_NDEF(Field45);
	}
	
	rlt = TEST(46); // Field 46 XGD 
	if(OK == rlt){
		dst->Field46 = curpos;
		dst->Field46_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field46_l);MEMVIOLATIONCHECK
		PRINT(Field46);
	}else if(FAIL == rlt){
		PRINT_F(Field46);
	}else{
		PRINT_E(Field46);
	}

	rlt = TEST(47); // Field47
	if(OK == rlt){
		dst->Field47 = curpos;
		dst->Field47_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field47_l);MEMVIOLATIONCHECK
		PRINT(Field47);
	}else if(FAIL == rlt){
		PRINT_F(Field47);
	}else{
		PRINT_E(Field47);
	}

	rlt = TEST(48); //Field 48
	if(OK == rlt){
		dst->Field48 = curpos;
		dst->Field48_l = 2 + calLenFieldOfCompressedAlign(curpos, 2, BCD);
		curpos += (dst->Field48_l);MEMVIOLATIONCHECK
		PRINT(Field48);
	}else if(FAIL == rlt){
		PRINT_F(Field48);
	}else{
		PRINT_E(Field48);
	}

	rlt = TEST(49); //Field49
	if(OK == rlt){
		dst->Field49 = curpos;
		curpos += (dst->Field49_l = 3);MEMVIOLATIONCHECK
		PRINT(Field49);
	}else if(FAIL == rlt){
		PRINT_F(Field49);
	}else{
		PRINT_E(Field49);
	}

	rlt = TEST(50); //Field 50
	if(OK == rlt){
		dst->Field50 = curpos;
		dst->Field50_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field50_l);MEMVIOLATIONCHECK
		PRINT(Field50);
	}else if(FAIL == rlt){
		PRINT_F(Field50);
	}else{
		PRINT_E(Field50);
	}

	if(TEST(51)){ // Field 51 is not defined.
		PRINT_NDEF(Field51);
	}

	rlt = TEST(52); //Field52
	if(OK == rlt){
		dst->Field52 = curpos;
		curpos += (dst->Field52_l = 8);MEMVIOLATIONCHECK
		PRINT(Field52);
	}else if(FAIL == rlt){
		PRINT_F(Field52);
	}else{
		PRINT_E(Field52);
	}

	rlt = TEST(53); //Field53
	if(OK == rlt){
		dst->Field53 = curpos;
		curpos += (dst->Field53_l = 8);MEMVIOLATIONCHECK
		PRINT(Field53);
	}else if(FAIL == rlt){
		PRINT_F(Field53);
	}else{
		PRINT_E(Field53);
	}

	rlt = TEST(54); //Field 54
	if(OK == rlt){
		dst->Field54 = curpos;
		dst->Field54_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field54_l);MEMVIOLATIONCHECK
		PRINT(Field54);
	}else if(FAIL == rlt){
		PRINT_F(Field54);
	}else{
		PRINT_E(Field54);
	}

	rlt = TEST(55); //Field 55
	if(OK == rlt){
		dst->Field55 = curpos;
		dst->Field55_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field55_l);MEMVIOLATIONCHECK
		PRINT(Field55);
	}else if(FAIL == rlt){
		PRINT_F(Field55);
	}else{
		PRINT_E(Field55);
	}

	rlt = TEST(56); //Field 56
	if(OK == rlt){
		dst->Field56 = curpos;
		dst->Field56_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field56_l);MEMVIOLATIONCHECK
		PRINT(Field56);
	}else if(FAIL == rlt){
		PRINT_F(Field56);
	}else{
		PRINT_E(Field56);
	}

	if(TEST(57)){ // Field 57 is not defined.
		PRINT_NDEF(Field57);
	}

	rlt = TEST(58); //Field 58
	if(OK == rlt){
		dst->Field58 = curpos;
		dst->Field58_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field58_l);MEMVIOLATIONCHECK
		PRINT(Field58);
	}else if(FAIL == rlt){
		PRINT_F(Field58);
	}else{
		PRINT_E(Field58);
	}

	rlt = TEST(59); //Field 59
	if(OK == rlt){
		dst->Field59 = curpos;
		dst->Field59_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field59_l);MEMVIOLATIONCHECK
		PRINT(Field59);
	}else if(FAIL == rlt)	{
		PRINT_F(Field59);
	}else{
		PRINT_E(Field59);
	}

	rlt = TEST(60); //Field 60
	if(OK == rlt){
		dst->Field60 = curpos;
		dst->Field60_l = 2 + calLenFieldOfCompressedAlign(curpos, 2, BCD);
		curpos += (dst->Field60_l);MEMVIOLATIONCHECK
		PRINT(Field60);
	}else if(FAIL == rlt){
		PRINT_F(Field60);
	}else{
		PRINT_E(Field60);
	}

	rlt = TEST(61); //Field 61
	if(OK == rlt){
		dst->Field61 = curpos;
		dst->Field61_l = 2 + calLenFieldOfCompressedAlign(curpos, 2, BCD);
		curpos += (dst->Field61_l);MEMVIOLATIONCHECK
		PRINT(Field61);
	}else if(FAIL == rlt){
		PRINT_F(Field61);
	}else{
		PRINT_E(Field61);
	}

	rlt = TEST(62); //Field 62
	if(OK == rlt){
		dst->Field62 = curpos;
		dst->Field62_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field62_l);MEMVIOLATIONCHECK
		PRINT(Field62);
	}else if(FAIL == rlt){
		PRINT_F(Field62);
	}else{
		PRINT_E(Field62);
	}

	rlt = TEST(63); //Field 63
	if(OK == rlt){
		dst->Field63 = curpos;
		dst->Field63_l = 2 + calLenField(curpos, 2, BCD);
		curpos += (dst->Field63_l);MEMVIOLATIONCHECK
		PRINT(Field63);
	}else if(FAIL == rlt){
		PRINT_F(Field63);
	}else{
		PRINT_E(Field63);
	}

	rlt = TEST(64);
	if(OK == rlt){
		dst->Field64 = curpos;
		curpos += (dst->Field64_l = 8);MEMVIOLATIONCHECK
		PRINT(Field64);
	}else if(FAIL == rlt){
		PRINT_F(Field64);
	}else{
		PRINT_E(Field64);
	}
#undef TEST
#undef PRINT
#undef PRINT_F
#undef PRINT_E
#undef PRINT_NDEF
#undef MEMVIOLATIONCHECK
    return OK;
}