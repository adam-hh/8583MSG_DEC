#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"
int main(int argc, char **argv)
{
	char MsgDef[] = "00d460000100003060100000000301003020048000c28031030000000000000001000018032006363030303431363938343936343032353039344130303100316d63633d343630266d6e633d30266c61633d39333430266369643d3439373331353600964132303638303130303230323032303139303030303034303247323130303130303034383033303036303030313438303430303844304136414134443035303038333230303034202041333031383133353332353632383431373030303134380014100000120000014645414341433734ac";
	u8 *Msg = NULL;
	if(argc == 2) //get message from stdin
	{
		Msg = (u8*)malloc((strlen(argv[1])) / 2);
		memset(Msg, 0, (strlen(argv[1])) / 2));
		if(!HexviewcharToHex(argv[1], Msg, (strlen(argv[1])) / 2))
			{
				printf("illegal hex string.\n");
				return 0;
			}
		size_t MsgLen = (strlen(argv[1])) / 2;
		if(CalSmalMemoryPieceVal(Msg, 2) != (MsgLen - 2)) 
		{
			printf("message lengh check failed,usage: %s <message body>\ndecoded length is %u, actual string length of the message is %lu\n", argv[0], CalSmalMemoryPieceVal(Msg, 2), MsgLen - 2);
			return 0;
		} else printf("decodeing...\n");
	} 
	else //get default message
	{
		Msg = (u8*)malloc((strlen(MsgDef)) / 2);
		memset(Msg, 0, (strlen(MsgDef) / 2));
		if(!HexviewcharToHex(MsgDef, Msg, (strlen(MsgDef)) / 2))
			{
				printf("illegal hex string.\n");
				return 0;
			}
		size_t MsgLen = (strlen(MsgDef)) / 2;
		if(CalSmalMemoryPieceVal(Msg, 2) != (MsgLen - 2)) 
		{
			printf("message lengh check failed,usage: %s <message body>\ndecoded length is %u, actual string length of the message is %lu\n", argv[0], CalSmalMemoryPieceVal(Msg, 2), MsgLen - 2);
			return 0;
		} else printf("decodeing...\n");		
	}

	size_t MsgLen = CalSmalMemoryPieceVal(Msg, 2) + 2;
	Decoder MsgDecoder = {0};
	if(!DecodeMemMsg(Msg, &MsgDecoder, MsgLen))
		{
			printf("message decode failed.\n");
			return 0;
		}	
	return 1;
}
