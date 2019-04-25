/*--------------
Author Huhui(Adam)
Head file for 8583 project
----------------*/
#ifndef _8583_
#define _8583_
#include "primitive_type.h"
#define OK 1
#define FAIL 0
#define FATAL_ERROR -1
#define CONSOLEBUFSIZE 10240
#define TMPMSGSIZE 100
#define MAXMSGSIZE 1024
#define VERB "B001"  // major version

typedef enum {RIGHT, LEFT, DEFAULT} HowNumberAlign;
//Digit encode format descriptor
typedef enum{
	NOFORMAT = 0,
	NOFORMAT_BIGEDIAN,
	NOFORMAT_LITEDIAN,
	BCD,
	BCDLALIGN,
	BCDRALIGN,
	ASCII
}DIGITENCODEFORMAT;

//JL 8583 message body
#define F(x) const u8* Field##x; u32 Field##x##_l
typedef struct{
	u8 MsgLen[2];
	u8 MsgTPDU[5];
	u8 MsgHead[6];
	u8 Field0[3];
	const u8* Field1; u32 Field1_l;F(2);F(3);F(4);F(5);F(6);F(7);F(8);F(9);F(10);
	F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
	F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);
	F(31);F(32);F(33);F(34);F(35);F(36);F(37);F(38);F(39);F(40);
	F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
	F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);
	F(61);F(62);F(63);F(64);
}MsgJL;
#undef F

//Console buffer to send messages to GUI side
typedef struct{
	s8 buf[CONSOLEBUFSIZE];
	s8* w;
}ConsoleBuf;
extern ConsoleBuf consolebuffer;
extern int initConsoleBuf();
extern int clearConsoleBuf();
extern int printConsole(s8*);

//Decode engine
extern int DecodeJLMsg(const u8 *src, u32 len, MsgJL* dst);

//tools
extern u32 calLenField(const u8* src, u32 len, DIGITENCODEFORMAT flag);
extern u32 calLenFieldOfCompressedAlign(const u8* src, u32 len, DIGITENCODEFORMAT flag);
extern int HexcharStringRevert(const s8* src, u8* dst, u32 len);
extern void printMem(const u8*, u32);
extern void printMemS(const u8* src, u32 len, s8* dst);
extern int BitMaptest(const int, const u8*, u32);

#undef F
#endif
