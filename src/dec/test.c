#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"

int main()
{
    /*
    setbuf(stdout, NULL);
    if(initConsoleBuf())
        printf("initConsoleBuf sucess.\n");
    printConsole("Hello");
    printConsole(" World.\n");
    printf("%s", consolebuffer.buf);
    if(clearConsoleBuf())
        printf("clearConsoleBuf sucess.\n");
    printConsole("Again Hello\n");
    printConsole(" World.\n");
    printf("%s", consolebuffer.buf);

    u8 data[18] = {0};
    clearConsoleBuf();
    HexcharStringRevert("010203040506010203040506010203040506", data, 18);
    PrintMem(data, 18);
    u8 dd[] = {0x31, 0x32, 0x33, 0x34};
    printf("Length 4, noformat:%u\n", calLenField(dd, 4, NOFORMAT));
    printf("Length 3, noformat:%u\n", calLenField(dd, 3, NOFORMAT));
    printf("Length 2, noformat:%u\n", calLenField(dd, 2, NOFORMAT));
    printf("Length 1, noformat:%u\n", calLenField(dd, 1, NOFORMAT));
    printf("Length 4, BCD format:%u\n", calLenField(dd, 4, BCD));
    printf("Length 2, BCD format:%u\n", calLenField(dd+1, 3, BCD));
    printf("Length 1, BCD format:%u\n", calLenField(dd, 1, BCD));
    printf("Length 1, BCD format with compressed align:%u\n", calLenFieldOfCompressedAlign(dd, 1, BCD));
    printf("Length 4, ascii format:%u\n", calLenField(dd, 4, ASCII));
    printf("%s", consolebuffer.buf);

    clearConsoleBuf();
    */
    char MsgDef[] = "00d460000100003060100000000301003020048000c28031030000000000000001000018032006363030303431363938343936343032353039344130303100316d63633d343630266d6e633d30266c61633d39333430266369643d3439373331353600964132303638303130303230323032303139303030303034303247323130303130303034383033303036303030313438303430303844304136414134443035303038333230303034202041333031383133353332353632383431373030303134380014100000120000014645414341433734ac";
    u32 len = (sizeof(MsgDef) -1) / 2;
    u8* msg = (u8*)malloc(len);
    HexcharStringRevert(MsgDef, msg, len);
    PrintMem(msg, len);
    u32 tpduLen = 0;
    s8 tpdu[] = "6010000000";
    if(NULL != TestTPDU(CUSTOMER_JL, tpdu, msg, len, &tpduLen)){
        printf("%s test pass, message length is %u\n", tpdu, tpduLen);
    }else{
        printf("%s test failed.\n", tpdu);
    }

    MsgJL jl = {0};
    if(OK == DecodeJLMsg(msg, len, &jl)){
        printf("DecodeJLMsg sucess.\n");
        printf("%s", ConsoleBuffer.buf);
    }
    free(msg);
}