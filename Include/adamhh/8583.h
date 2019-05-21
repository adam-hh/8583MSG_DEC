/*
 *Author Huhui(Adam)
 *Head file for 8583 project
 */
#ifndef H_8583
#define H_8583
#include <ctype.h>
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
}DigitEncodeFormat;
//Customer list
typedef enum{
    CUSTOMER_JL = 0,
    CUSTOMER_CUP = 1,
    CUSTOMER_YS = 2
}CustomerID;

//JL 8583 message body
#define F(x) const u8* field##x; u32 field##x##_l
typedef struct{
    u8 msgLen[2];
    u8 msgTPDU[5];
    u8 msgHead[7];
    u8 field0[2];
    F(1); F(2); F(3); F(4); F(5); F(6); F(7); F(8); F(9); F(10);
    F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
    F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);
    F(31);F(32);F(33);F(34);F(35);F(36);F(37);F(38);F(39);F(40);
    F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
    F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);
    F(61);F(62);F(63);F(64);
}MsgJL;
//YS 8583 message body
typedef struct{
    u8 msgLen[2];
    u8 msgTPDU[5];
    u8 msgHead[26];
    u8 field0[2];
    F(1); F(2); F(3); F(4); F(5); F(6); F(7); F(8); F(9); F(10);
    F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
    F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);
    F(31);F(32);F(33);F(34);F(35);F(36);F(37);F(38);F(39);F(40);
    F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
    F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);
    F(61);F(62);F(63);F(64);
}MsgYS;
#undef F
//Union of TPDU, for data align usage
typedef union{
    u64 val;
    u8 tpdu[5];
}TPDUUnion;

//Console buffer to send messages to GUI side
typedef struct{
    s8 buf[CONSOLEBUFSIZE];
    s8* w;
}ConsoleBuf;
extern ConsoleBuf ConsoleBuffer;
extern int InitConsoleBuf();
extern int ClearConsoleBuf();
static inline int PrintConsole(const s8* str)
{
    if(strlen(str) >= CONSOLEBUFSIZE)
        return -1;
    if((strlen(ConsoleBuffer.buf) + strlen(str)) < CONSOLEBUFSIZE){
        strcat(ConsoleBuffer.buf, str);
        return 1;
    }else{
        ClearConsoleBuf();
        strcat(ConsoleBuffer.buf, str);
        return 0;
    }
}

//Decode engine
extern int DecodeJLMsg(const u8 *src, u32 len, void* dst);
extern int DecodeYSMsg(const u8 *src, u32 len, void* dst);

//basic tools
/*
 *hex char to int, chr is [0-9A-Za-z], return [0x0-0xF]
 */
static inline u8 Xtoi(u8 chr)
{
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
/*
 *reverse the byte sequnce in src.
 */
static inline void* ToLittleEndian(const void* src, size_t len, void* dst)
{
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
static inline void* ToBigEndian(const void* src, size_t len, void* dst)
{
    return ToLittleEndian(src, len, dst);
}
extern int BCDEncode(const char* src, size_t len, void* dst, DigitEncodeFormat align); 
extern int BCDDecode(const void* src, size_t len, char* dst);

extern void PrintMem(const u8*, u32);//format mem to stdout
static inline void PrintMemS(const u8* src, u32 len, s8* dst) //format mem to dst
{
    if( len > (MAXMSGSIZE / 2)){
        PrintConsole("dst too short.\n");
        memset(dst, 0, strlen(dst));
        return;
    }
    for(const u8* p = src; p - src != len; p++){
        sprintf(dst,"%.2x", *p);
        dst += 2;
    }
}
extern int HexcharStringRevert(const s8* src, u8* dst, u32 len);//revert hex char string to the original binary data
/*
 *description: test if a tpdu string exist in a message body.
 *parameter input cid: CUSTOMERID
 *parameter input tpdu: tpdu string
 *parameter input src: memory block to be tested.
 *parameter input len: memory block length.
 *parameter output dstLen: TPDU memory block length
 *return: TPDU memory block's address
 */
static inline const u8* TestTPDU(CustomerID cid, const s8 *tpdu, const u8 *src, u32 len, u32 *dstLen)
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
            ToLittleEndian(p-2, 2, &tmplen);
            switch(cid){
                case CUSTOMER_JL:
                    *dstLen = 2 + tmplen;
                    rightVCheck = p + tmplen -src;
                    break;
                case CUSTOMER_YS:
                    *dstLen = tmplen;
                    rightVCheck = p + tmplen -src - 2;
                    break;
                default: 
                    *dstLen = 2 + tmplen;
                    rightVCheck = p + tmplen -src;
                    break;
            }
            if(rightVCheck > len) //check right side violation
                return NULL;
            if((p - src) >= 2) //check left side violation
                return p - 2;
        }
    }
    return NULL;
}
/*
 *calculate the length value of a field.
 *note: accuracy may overflow from unsigned int
 */
static inline u32 CalLenField(const u8* src, u32 len, DigitEncodeFormat flag)
{
    u32 val = 0;
    switch(flag){
        case NOFORMAT:
        case NOFORMAT_BIGEDIAN:{
            u32 val = 0;
            if(len > 4)
                return 0;
            u8* p = (u8*)&val;
            memcpy(p+4-len, src, len);
            ToLittleEndian(&val, sizeof(val), &val);
            return val;
        }
        case NOFORMAT_LITEDIAN:{
            u32 val = 0;
            if(len > 4)
                return 0;
            u8* p = (u8*)&val;
            memcpy(p+4-len, src, len);
            return val;
        }
        case BCD:
        case BCDRALIGN:{
            s8 bcdEncoded[2 * len + 1];
            memset(bcdEncoded, 0, 2 * len +1);
            if(BCDDecode(src, len, bcdEncoded) != 1)
                return 0;
            s8 *ptr = bcdEncoded;
            for(val = 0; isdigit(*ptr); ptr++){//simplified atoi
                val = 10 * val + (*ptr - 0x30);
            }
            return val;
        }
        case BCDLALIGN:
            PrintConsole("BCD digit left align is not supported.\n");
            return 0;
        case ASCII:{
            const u8* p = src;
            for(val = 0; p != (src + len); p++){
                if(isdigit(*p))
                    val = 10 * val + (*p - 0x30);
                else
                    return 0;
            }
            return val;
        }
        default:
            PrintConsole("illegal digit encode format specified.\n");
            return 0;
    }
}
/*
 *some special length field need to be cut and align.
 */
static inline u32 CalLenFieldOfCompressedAlign(const u8* src, u32 len, DigitEncodeFormat flag)
{
    u32 rlt = CalLenField(src, len, flag);
    if(!rlt)
        return rlt;
    if((rlt % 2) == 1){
        return (rlt + 1) / 2;
    }else{
        return rlt / 2;
    }
}
/*
 *src is the bitmap, index is the target to be tested.
 */
static inline int BitMaptest(const int index, const u8 *src, u32 len)
{
    if((index < 1) || (index > (8*len)))
        {
            s8 tmpmsg[TMPMSGSIZE] = {0};
            sprintf(tmpmsg, "ivalid index %d or bitmap length %u \n", index, len);
            PrintConsole(tmpmsg);
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
            PrintConsole("bitmap index error \n"); 
            return FATAL_ERROR;
    }
    u32 i = ((index -1) / 8);
    if(chr & *(src + i))
        {return OK;}
    return FAIL;
}
typedef enum{//work with msgScan func
    L_FIXED,//fixed length
    L_BCD,//normal BCD encoded length
    L_BCD_COMPRESS_ALIGN,//BCD encoded but compressed and aligned
    L_UNDEFINE//undefine field
}LengthFieldFlag;
/*
 *description:scan a field specified by an index number
 *parameter output fld: field address 
 *parameter input fldname: field name
 *parameter output fldlen: field length
 *parameter input bmp: bitmap address
 *parameter input bmplen: bitmap length
 *parameter input idx: index
 *parameter input and output cur: cursor pointer
 *parameter input flen: field length
 *parameter input flag: a flag
 *return: the result
 */
static inline int MsgScan(const u8 **fld, const s8 *fldname, u32 *fldlen, const u8 *bmp, u32 bmplen, 
        int idx, const u8 **cur, u32 flen, LengthFieldFlag flag)
{
    int rlt = BitMaptest(idx, bmp, bmplen);
    s8 tmpmsg[MAXMSGSIZE] = {0};
    s8 tmp[MAXMSGSIZE + TMPMSGSIZE] = {0};
    if(OK == rlt){
        *fld = *cur;
        switch(flag){
            case L_FIXED:{
                *fldlen = flen;
                *cur += *fldlen;
                break;
            }
            case L_BCD:{
                *fldlen = flen + CalLenField(*fld, flen, BCD);
                *cur += *fldlen;
                break;
            }
            case L_BCD_COMPRESS_ALIGN:{
                *fldlen = flen + CalLenFieldOfCompressedAlign(*fld, flen, BCD);
                *cur += *fldlen;
                break;
            }
            case L_UNDEFINE:{
                sprintf(tmp, "fatal error: undefined %s found.\n", fldname);
                PrintConsole(tmp);
                return FATAL_ERROR;
            }
            default:{
                sprintf(tmp, "fatal error(unknow) when processing %s.\n", fldname);
                PrintConsole(tmp);
                return FATAL_ERROR;
            }
        }
        PrintMemS(*fld, *fldlen, tmpmsg);
        sprintf(tmp, "%s:%s\n", fldname, tmpmsg);
        PrintConsole(tmp);
        return OK;
    }else if(FAIL == rlt){
        sprintf(tmp, "%s: not exist.\n", fldname);
        PrintConsole(tmp);
        return FAIL;
    }else{
        sprintf(tmp, "fatal error(unknow) when processing %s.\n", fldname);
        PrintConsole(tmp);
        return FATAL_ERROR;
    }
}

#ifdef __cplusplus
}
#endif
#endif