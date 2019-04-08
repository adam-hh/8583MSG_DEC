#include "8583dump.h"
extern void callbackPrintNextPackage(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);
extern void callbackWriteToBuff(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);
int main()
{
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	setbuf(stdin, NULL);
	#ifdef WIN32
	/* Load Npcap and its functions. */
	if (!LoadNpcapDlls())
	{
		fprintf(stderr, "Couldn't load Npcap\n");
		return -1;
	}
	printf("Npcap Loaded\n");
	#endif
	if(printDevList() == 1)
		printf("printDevList sucess.\n");
	else
		printf("printDevList failed.\n");

	if(openDev() == 1)
		printf("openDev sucess.\n");
	else
		printf("openDev failed.\n");
	
	printf("Keyin the filter:\n");
	s8 infilter[100] = {0};
	fflush(stdin);
	gets(infilter);
	if(setFilter(infilter) == 1)
		printf("setFilter %s sucess.\n", infilter);
	else
		printf("setFilter %s failed.\n", infilter);
	
	/*while(1)
	{
		int rlt = printNextPackage();
		if(-1 == rlt)
		{
			printf("printNextPackage failed.\n");
			break;
		}
	}*/
	/*
	s8 in[100] = {0};
	printf("Keyin a filter, example: \"tcp port 443\"\n");
	fflush(stdin);
	gets(in);
	if(-1 == printTCPTraffic(in))
		printf("printTCPTraffic failed.\n");
	if(PHandle.handle != NULL)
		pcap_close(PHandle.handle);
	*/
	//loop(0, callbackPrintNextPackage, NULL);	
	userBuff *usbf;
	if((usbf = initUserBuff(100)) == NULL)
		printf("initUserBuff(100) failed.\n");
	printf("initUserBuff(100) sucessed.\n");

	printf("Keyin howmany packages to capture:\n");
	fflush(stdin);
	int in;
	scanf("%d", &in);
	// fflush(stdin);
	loop(in, callbackWriteToBuff, (u8*)usbf);

	tcpDataBlock tdbk = {0};	
	while(1)
	{
		int rlt = readFromUserBuff(usbf, &tdbk);
		if(rlt ==  1)
		{
			printf("readFromUserBuff sucess.\n");
			printf("%s\n", tdbk.extraInfo);			
			/* print ip addresses and udp ports */
		    printf("%d.%d.%d.%d:%d --> %d.%d.%d.%d:%d\n",
			    tdbk.ipSrc.byte1,
			    tdbk.ipSrc.byte2,
		    	tdbk.ipSrc.byte3,
		    	tdbk.ipSrc.byte4,
		    	tdbk.portSrc,
		    	tdbk.ipDst.byte1,
			    tdbk.ipDst.byte2,
			    tdbk.ipDst.byte3,
			    tdbk.ipDst.byte4,
			    tdbk.portDst);
			/* Print the packet */
			printf("Data:\n");
			for (int i=1; (i < tdbk.dataLen + 1 ) ; i++)
			{
				printf("%.2x ", tdbk.data[i-1]);
				if ( (i % 16) == 0) printf("\n");
			}
			printf("\n\n");
			free(tdbk.data);
			memset(&tdbk, 0, sizeof(tcpDataBlock));
		}
		else if(rlt == 0)
			{
				printf("Empty userBuff.\n");
				break;
			}
		else
			{
				printf("Unkown error occured readFromuserBuff.\n");
				break;
			}
	}
	if(releaseUserBuff(usbf))
		printf("releaseUserBuff sucess.\n");
	return 1;
}
