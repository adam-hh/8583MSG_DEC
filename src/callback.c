#include "8583dump.h"
#define LINE_LEN 16
/*Function:loop
 *Description:loop the traffic datas of PHandle, 
 *Input:cnt, callback, user
 *Output:no
 *Return: 0 if cnt is exhausted or if, when reading from a ``savefile'', no more packets are available; 
 *-1 if an error occurs or -2 if the loop terminated due to a call to pcap_breakloop() before any packets were processed.
 *Other:  the callback function is
 *typedef void (*pcap_handler)(u_char *, const struct pcap_pkthdr *,
	const u_char *);
 *user* passed to u_char*, pcap_pkthdr* point to the current pcap_pkthdr, u_char*  point to the data memory first byte address
 */
int loop(int cnt, pcap_handler callback, u8 *user)
{	
    if((PHandle.handle == NULL) || (PHandle.avaliable == -1))
		{
			printf("PHandle is not a live handle.\n");
			return -1;
		}
	return pcap_loop(PHandle.handle, cnt, callback, user);
}

void callbackPrintNextPackage(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data)
{
    struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;

	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);
	/* print pkt timestamp and pkt len */
	printf("%s,%.6ld len:%d\n", timestr, header->ts.tv_usec, header->len);
    /* Print the packet */
	for (int i=1; (i < header->caplen + 1 ) ; i++)
	{
		printf("%.2x ", pkt_data[i-1]);
		if ( (i % LINE_LEN) == 0) printf("\n");
	}		
	printf("\n\n");	
}

void callbackWriteToBuff(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data)
{
	tcpDataBlock tb = {0};
	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;

	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);
	/* print pkt timestamp and pkt len */
	sprintf(tb.extraInfo, "%s,%.6ld len:%d", timestr, header->ts.tv_usec, header->len);

	
	IP_HEADER *ih;
    TCP_HEADER *tcph;
    ih = (IP_HEADER *) (pkt_data + sizeof(ETH_HEADER));
    int ip_len = (ih->ver_iphl & 0xF) * 4;
    tcph = (TCP_HEADER *)((u8 *)ih + ip_len);
    u16 sport = tcph->sport;
    u16 dport = tcph->dport;
    sport = ((sport & 0x00FF) << 8) + ((sport & 0xFF00) >> 8); 
    dport = ((dport & 0x00FF) << 8) + ((dport & 0xFF00) >> 8);

	tb.ipSrc = ih->ip_src;
	tb.ipDst = ih->ip_dst;
	tb.portSrc = sport;
	tb.portDst = dport;
	tb.data = pkt_data;
	tb.dataLen = header->caplen;

	int rlt = writeToUserBuff((userBuff *)param, &tb, header->caplen);
	if(-1 == rlt)
		fprintf(stderr, "writeToUserBuff fatal error occured.\n");
}