#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "8583.h"
int main(int argc, char **argv)
{
	char MsgDef[] = "00d460000100003060100000000301003020048000c28031030000000000000001000018032006363030303431363938343936343032353039344130303100316d63633d343630266d6e633d30266c61633d39333430266369643d3439373331353600964132303638303130303230323032303139303030303034303247323130303130303034383033303036303030313438303430303844304136414134443035303038333230303034202041333031383133353332353632383431373030303134380014100000120000014645414341433734ac";
	char *Msg = NULL;
	if(argc == 2) 
	{
		Msg = argv[1];
		if(strlen(Msg) < 4) 
		{
			printf("message lengh is too short(4 characters at least),usage: %s <message body>\n", argv[0]);
			return 0;
		}
		u8 MemMsglen[2] = {0};
		HexviewcharToHex(Msg, MemMsglen, 2);
		//unsigned int len = (((unsigned int)MemMsglen[0]) << 1) + ((unsigned int)MemMsglen[1]);
		unsigned int len = CalSmalMemoryPieceVal(MemMsglen, 2);
		if(strlen(Msg) != ((len + 2)*2)) 
		{
			printf("message lengh check failed,usage: %s <message body>\ndecoded length is %d, actual string length of the message is %lu\n", argv[0], len +2, strlen(Msg)/2);
			return 0;
		} else printf("decodeing...\n");
	} else 
	{
		Msg = MsgDef;
		printf("usage: %s <message body>\nno valid user input, i will decode the default message following: \n%s\n", argv[0], Msg);		
	}

	unsigned int MsgmemLen = (strlen(Msg))/2;
	unsigned char *Msgmem = (unsigned char*)malloc(MsgmemLen);
	memset(Msgmem, 0x0, MsgmemLen);
	for(int i = 0; i < MsgmemLen; i++)
	{
		if(!(HexviewcharToHex(Msg, Msgmem, MsgmemLen))) printf("error occured.\n");
	}
	printf("-----memory view begin-----\n");
	for( int i = 0; i < MsgmemLen; i++)
	{
		printf("%02x ", *(Msgmem + i));
		if(((i + 1) % 16) == 0) printf("\n");
	}
	if((MsgmemLen % 16) != 0)
		printf("\n");
	printf("-----memory view end-------\n");
	/*
	char *MsgTest = (char*)malloc((MsgmemLen*2) + 1);
	memset(MsgTest, 0x0, (MsgmemLen*2) + 1);
	for(int i = 0; i < MsgmemLen; i++)
	{
		if(!Hexview(Msgmem + i, MsgTest + (i*2), MsgTest + (i*2) + 1)) printf("fatal error occured.\n");
	}
	printf("Hex view:\n%s\n", MsgTest);
	*/
	free(Msgmem);
	//free(MsgTest);

	int curpos = 0;
	char MsgLen[5] = {0};
	char MsgTPDU[11] = {0};
	char MsgType[5] = {0};
	char MsgBitMap[17] = {0};
	char MsgType2[9] = {0}; // length known
	char MsgField2[43] = {0}; //length var
	char MsgField3[13] = {0}; //length known
	char MsgField4[25] = {0};// length known
	char MsgField5[33] = {0}; //length known
	char MsgField6[9] = {0}; //length known
	char MsgField7[1] = {0}; // not specified
	//....
	char MsgField11[13] = {0}; //length known
	char MsgField12[13] = {0}; //length known
	char MsgField13[9] = {0}; //length known
	char MsgField14[9] = {0};  //length known
	char MsgField15[9] = {0}; //length known
	char MsgField16[1] = {0};// not specified
	//...
	char MsgField22[7] = {0}; //length known
	char MsgField23[7] = {0}; //length known
	char MsgField24[1] = {0}; // not specified
	char MsgField25[5] = {0}; //length known
	char MsgField26[5] = {0}; //length known
	char MsgField27[1] = {0}; //not specified
	//...
	char MsgField32[27] ={0};// length var
	char MsgField33[1] ={0};// not specified
	//...
	char MsgField35[79] = {0};//length var
	char MsgField36[215] = {0};//length var
	char MsgField37[25] = {0};//length known
	char MsgField38[13] = {0};//length known
	char MsgField39[5] = {0}; //length known
	char MsgField40[1] = {0};//not specifield
	char MsgField41[17] = {0};// length known
	char MsgField42[31] = {0}; //length known
	char MsgField43[1] = {0}; // not specified
	char MsgField44[45] = {0}; //length var
	char MsgField45[1] = {0}; //not specified
	char MsgField46[2004] = {0}; //length var
	char MsgField47[47] = {0};// length var
	char MsgField48[2004] = {0}; // length var
	char MsgField49[7] = {0}; //length known
	char MsgField50[2004] = {0}; //length var
	char MsgField51[1] = {0}; //not specifield
	char MsgField52[17] = {0}; //length known
	char MsgField53[33] = {0}; // length known
	char MsgField54[47] = {0}; //length var
	char MsgField55[2004] = {0}; //length var
	char MsgField56[47] = {0}; //length var
	char MsgField57[1] = {0};// not specified
	char MsgField58[207] = {0}; //length var
	char MsgField59[2004] = {0};// length var 
	char MsgField60[41] = {0}; //length var
	char MsgField61[65] = {0}; //length var
	char MsgField62[1031] = {0}; //length var
	char MsgField63[333] = {0}; // length var
	char MsgField64[17] = {0}; //length known
	
	
	strncpy(MsgLen, Msg + curpos, 4);curpos += 4;printf("Message Length:0x%s\n", MsgLen);
	strncpy(MsgTPDU, Msg + curpos, 10); curpos += 10;printf("Message TPDU:0x%s\n", MsgTPDU);
	strncpy(MsgType, Msg + curpos,4); curpos += 4;printf("Message Type:0x%s\n", MsgType);
	strncpy(MsgBitMap, Msg + curpos, 16); curpos += 16;printf("Message BitMap:0x%s\n", MsgBitMap);
	//.......
	/*
	if(BitmapTest2(2, MsgBitMap)) 
	{
		char len[3] = {0};
		strncpy(len, Msg + curpos, 2);
		//curpos += 2;
		strncpy(MsgField2, Msg + curpos, 2 + BCDToInt(len));
		curpos += (2 + BCDToInt(len));
		printf("MsgField2:%s \n", MsgField2);
	} else printf("MsgField2 was not marked in the map.\n");
	//......
	if(BitmapTest2(59, MsgBitMap))
	{
		printf("MsgField59 was found. pending decode...\n");
		/*char len[5] = {0};
		strncpy(len, Msg + curpos, 4);
		strncpy(MsgField59, Msg + curpos, 4 + BCDToInt(len));
		curpos += (4 + BCDToInt(len));
		printf("MsgField59:%s", MsgField59);
	} else printf("MsgField59 was not found.\n");
	*/
	return 1;
}
