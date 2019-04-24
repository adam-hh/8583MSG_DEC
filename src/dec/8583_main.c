#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"
#define VERL "S001" //minor version
int main(int argc, char **argv)
{
	initConsoleBuf();
	char MsgDef[] = "00d460000100003060100000000301003020048000c28031030000000000000001000018032006363030303431363938343936343032353039344130303100316d63633d343630266d6e633d30266c61633d39333430266369643d3439373331353600964132303638303130303230323032303139303030303034303247323130303130303034383033303036303030313438303430303844304136414134443035303038333230303034202041333031383133353332353632383431373030303134380014100000120000014645414341433734ac";
	u8 *Msg = NULL;
	if(argc == 2) //get message from stdin
	{
		if(strcmp(argv[1], "-version") == 0)
		{
			printf(VERB VERL "\n");
			return 1;
		}
		else
		{
			Msg = (u8*)malloc((strlen(argv[1])) / 2);
			memset(Msg, 0, (strlen(argv[1])) / 2);
			if(-1 == HexcharStringRevert(argv[1], Msg, strlen(argv[1]) / 2))
			{
				printf("illegal hex string, usage: %s <message body>\n", argv[0]);
				return 0;
			}
			u32 MsgLen = (strlen(argv[1])) / 2;
			if(calLenField(Msg, 2, NOFORMAT) != (MsgLen - 2)) 
			{
				printf("message lengh check failed,usage: %s <message body>\ndecoded length is %u, actual length of the message is %u\n", argv[0], calLenField(Msg, 2, NOFORMAT), MsgLen - 2);
				return 0;
			} else printf("decodeing...\n");
		}		
	} 
	else //get default message
	{
		Msg = (u8*)malloc((strlen(MsgDef)) / 2);
		memset(Msg, 0, (strlen(MsgDef) / 2));
		if(-1 == HexcharStringRevert(MsgDef, Msg, strlen(MsgDef) / 2))
			{
				printf("illegal hex string.\n");
				return 0;
			}
		u32 MsgLen = (strlen(MsgDef)) / 2;
		if(calLenField(Msg, 2, NOFORMAT) != (MsgLen - 2)) 
		{
			printf("message lengh check failed,usage: %s <message body>\ndecoded length is %u, actual length of the message is %u\n", argv[0], calLenField(Msg, 2, NOFORMAT), MsgLen - 2);
			return 0;
		} else
		{
			printf("no message specified, usage: %s <message body>\nspecify a message like:\n%s\n", argv[0], MsgDef);
			printf("decodeing...\n");
		}
	}
	printf("%s", consolebuffer.buf);
	clearConsoleBuf();
	u32 MsgLen = calLenField(Msg, 2, NOFORMAT) + 2;
	MsgJL decoder = {0};
	int rlt = DecodeJLMsg(Msg, MsgLen, &decoder);
	printf("%s", consolebuffer.buf);
	if(FAIL == rlt)
		{
			printf("message decode failed.\n");
			return 0;
		}
	else if(FATAL_ERROR == rlt)
		{
			printf("fatal error: memory vialation.\n");	
			return 0;
		}	
	return 1;
}
