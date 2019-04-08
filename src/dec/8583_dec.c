#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern unsigned char HexChrToInt(const char *hex);
extern int Hexview(unsigned char *in, char *h, char *l);
extern int BitmapTest2(int index, char *bitMap);
extern int BCDToInt( char *bcd );
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
		unsigned int len = 0;
		unsigned int lenfield[2] = {0};
		for( int i = 0; i < 2; i++)
		{
			lenfield[i] = (unsigned int)HexChrToInt(Msg + (i*2));
		}
		len = ((lenfield[0]) << 8) + lenfield[1];
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
		*(Msgmem + i) = HexChrToInt(Msg + (i*2));
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

	char *MsgTest = (char*)malloc((MsgmemLen*2) + 1);
	memset(MsgTest, 0x0, (MsgmemLen*2) + 1);
	for(int i = 0; i < MsgmemLen; i++)
	{
		if(!Hexview(Msgmem + i, MsgTest + (i*2), MsgTest + (i*2) + 1)) printf("fatal error occured.\n");
	}
	printf("Hex view:\n%s\n", MsgTest);

	free(Msgmem);
	free(MsgTest);

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
		printf("MsgField59:%s", MsgField59);*/
	} else printf("MsgField59 was not found.\n");
	
}

unsigned char HexChrToInt(const char *hex)
{
	unsigned char chr = 0x0;
	if( ((*hex) - 0x30) < 0xA ) chr = ((*hex) - 0x30)<<4;
	else switch(*hex)
	{
		case 'A': ;
		case 'a': chr = 0xA<<4;break;
		case 'B': ;
		case 'b': chr = 0xB<<4;break;
		case 'C': ;
		case 'c': chr = 0xC<<4;break;
		case 'D': ;
		case 'd': chr = 0xD<<4;break;
		case 'E': ;
		case 'e': chr = 0xE<<4;break;
		case 'F': ;
		case 'f': chr = 0xF<<4;break;
		default: printf("illegal hex symptom \n");
	}
	if( (*(hex+1) - 0x30) < 0xA ) chr += (*(hex+1) - 0x30);
	else switch(*(hex+1))
	{
		case 'A': ;
		case 'a': chr += 0xA;break;
		case 'B': ;
		case 'b': chr += 0xB;break;
		case 'C': ;
		case 'c': chr += 0xC;break;
		case 'D': ;
		case 'd': chr += 0xD;break;
		case 'E': ;
		case 'e': chr += 0xE;break;
		case 'F': ;
		case 'f': chr += 0xF;break;
		default: printf("illegal hex symptom \n");
	}	
	return chr;
}

int Hexview(unsigned char *in, char *h, char *l)
{
	unsigned char maskh, maskl, valh, vall;
	maskh = 0xF0;
	maskl = 0x0F;
	valh = ((*in) & maskh) >> 4;
	vall = ((*in) & maskl);
	if(valh < 0x0A) (*h) = valh + 0x30;
	else switch(valh)
	{
		case 0x0A: (*h) = 0x61; break;
		case 0x0B: (*h) = 0x62; break;
		case 0x0C: (*h) = 0x63; break;
		case 0x0D: (*h) = 0x64; break;
		case 0x0E: (*h) = 0x65; break;
		case 0x0F: (*h) = 0x66; break;
		default: return -1;
	}
	if(vall < 0x0A) (*l) = vall + 0x30;
	else switch(vall)
	{
		case 0x0A: (*l) = 0x61; break;
		case 0x0B: (*l) = 0x62; break;
		case 0x0C: (*l) = 0x63; break;
		case 0x0D: (*l) = 0x64; break;
		case 0x0E: (*l) = 0x65; break;
		case 0x0F: (*l) = 0x66; break;
		default: return -1;
	}
	return 1;
}

typedef struct{
unsigned char c[8];
}*BitMap, BitM;
BitMap StrToInt64( char *str )
{
	BitMap bm =  (BitMap)malloc(8);
	memset(bm, 0, 8);
	//bm.c[0] = char[0] + char[1]*16
	for( int i = 0; i<8; i++)
	{
		bm->c[i] = HexChrToInt(str + (i*2));
	}
	return bm;
}

BitMap CreateMask( int index )
{
	BitMap bm = (BitMap)malloc(8);
	memset(bm, 0, 8);
	int mod = index % 8;
	unsigned char chr = 0x0;
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
		default:printf("bitmap index error \n"); return NULL;
	}
	int i = index/8;
	bm->c[i] = chr;
	return bm;
}

int BitmapTest2(int index, char *bitMap)
{
	BitMap bitMapp = StrToInt64(bitMap);
	BitMap bitMask = CreateMask(index);
	int rlt = 0;
	for(int i = 0; i < 8; i++)
	{
		rlt += (bitMapp->c[i]) & (bitMask->c[i]);
	}
	free(bitMask);
	free(bitMapp);

	return rlt;
}

int BCDToInt( char *bcd )
{
	unsigned char *chr = (unsigned char*)malloc(strlen(bcd)/2);
	for(int i = 0; i < strlen(bcd); i++)
	{
		*(chr + i) = HexChrToInt(bcd + (i*2));
	}
	int val = 0;
	for(int j = strlen(bcd)/2, k = 0; j > 0; j--, k++)
	{
			val += *(chr + j -1);
			for( int l = 0; l < k; l++)
			{
				val *= 10;
			}	
	}
	free(chr);
	return val;
}
