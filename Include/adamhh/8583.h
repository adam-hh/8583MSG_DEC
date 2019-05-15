/*--------------
Author Huhui(Adam)
Head file for 8583 project
----------------*/
#ifndef _8583_
#define _8583_
#include "primitive_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OK 1
#define FAIL 0
#define FATAL_ERROR -1
#define CONSOLEBUFSIZE 10240
#define TMPMSGSIZE 100
#define MAXMSGSIZE 1024
#define VERB "B001"  // major version

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
//Customer list
typedef enum{
	CUSTOMER_JL = 0,
	CUSTOMER_CUP = 1,
	CUSTOMER_YS = 2
}CUSTOMERID;

//JL 8583 message body
#define F(x) const u8* Field##x; u32 Field##x##_l
typedef struct{
	u8 MsgLen[2];
	u8 MsgTPDU[5];
	u8 MsgHead[7];
	u8 Field0[2];
	const u8* Field1; u32 Field1_l;F(2);F(3);F(4);F(5);F(6);F(7);F(8);F(9);F(10);
	F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
	F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);
	F(31);F(32);F(33);F(34);F(35);F(36);F(37);F(38);F(39);F(40);
	F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
	F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);
	F(61);F(62);F(63);F(64);
}MsgJL;
//YS
typedef struct{
	u8 MsgLen[2];
	u8 MsgTPDU[5];
	u8 MsgHead[26];
	u8 Field0[2];
	const u8* Field1; u32 Field1_l;F(2);F(3);F(4);F(5);F(6);F(7);F(8);F(9);F(10);
	F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
	F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);
	F(31);F(32);F(33);F(34);F(35);F(36);F(37);F(38);F(39);F(40);
	F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
	F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);
	F(61);F(62);F(63);F(64);
}MsgYS;
#undef F
typedef union{
	u64 val;
	u8 tpdu[5];
}TPDUUnion;

//Console buffer to send messages to GUI side
typedef struct{
	s8 buf[CONSOLEBUFSIZE];
	s8* w;
}ConsoleBuf;
extern ConsoleBuf consolebuffer;
extern int initConsoleBuf();
extern int clearConsoleBuf();
static inline int printConsole(const s8* str)
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

//Decode engine
extern int DecodeJLMsg(const u8 *src, u32 len, void* dst);
extern int DecodeYSMsg(const u8 *src, u32 len, void* dst);

//basic tools
static inline u8 xtoi(u8 chr){//chr is [0-9A-Za-z], return [0x0-0xF]
    switch(chr){
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            return chr - 0x30;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            return chr - 0x57;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return chr - 0x37;
        default: return 0;
    }
}
static inline void* toLittleEndian(const void* src, size_t len, void* dst){//reverse the src, src and dst can be equal
	u8* chrsrc = (u8*)src;
    u8* chrdst = (u8*)dst + len -1;
    if(src != dst){
        while(chrsrc != (u8*)src + len){
            *chrdst-- = *chrsrc++;
        }
    }
	else{
        while(chrsrc != (u8*)src + (len / 2)){
            u8 tmp = *chrdst;
            *chrdst-- = *chrsrc;
            *chrsrc++ = tmp;
        }
	}
	return dst;
}
static inline void* toBigEndian(const void* src, size_t len, void* dst){//the same with toLittleEndian
	return toLittleEndian(src, len, dst);
}
extern int BCDEncode(const char* src, size_t len, void* dst, DIGITENCODEFORMAT align); 
extern int BCDDecode(const void* src, size_t len, char* dst);

//tools
extern const u8* testTPDU(CUSTOMERID cid, const s8 *TPDU , const u8 *src, u32 len, u32 *dstLen);//the TPDU block's address returned, dstLen is a output parameter of the TPDU block's length
extern u32 calLenField(const u8* src, u32 len, DIGITENCODEFORMAT flag);
extern u32 calLenFieldOfCompressedAlign(const u8* src, u32 len, DIGITENCODEFORMAT flag);
extern int HexcharStringRevert(const s8* src, u8* dst, u32 len);
extern int BitMaptest(const int index, const u8* src, u32 len);
extern void printMem(const u8*, u32);//format mem to stdout
static inline void printMemS(const u8* src, u32 len, s8* dst) //format mem to dst
{
    if( len > (MAXMSGSIZE / 2)){
        printConsole("dst too short.\n");
        memset(dst, 0, strlen(dst));
        return;
    }
    for(const u8* p = src; p - src != len; p++){
        sprintf(dst,"%.2x", *p);
        dst += 2;
    }
}

#ifdef __cplusplus
}
#endif

#endif
