#include "8583dump.h"
#define LINE_LEN 16

int PrintNextPackage()
{
    int res;
    struct pcap_pkthdr *header;
    const u8 *pkt_data;
    if((1 != PHandle.avaliable) || (PHandle.handle == NULL))
    {
        printf("PHandle is not a live handle.\n");
        return -1;
    }
    res = pcap_next_ex(PHandle.handle, &header, &pkt_data);
    if(res == 0)
    {
        return 0; //time out
    }
    if(res > 0)
    {
        struct tm *ltime;
        time_t local_tv_sec;
        char timestr[16];
        local_tv_sec = header->ts.tv_sec;
        ltime=localtime(&local_tv_sec);
        strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);		
        /* print pkt timestamp and pkt len */
        printf("%s,%.6ld len:%d \n", timestr, header->ts.tv_usec, header->len);

        /* Print the packet */
        for (int i=1; (i < header->caplen + 1 ) ; i++)
        {
            printf("%.2x ", pkt_data[i-1]);
            if ( (i % LINE_LEN) == 0) printf("\n");
        }
        printf("\n\n");	
        return 1;
    }
    if(res == -1)
    {
        printf("Error reading the packets: %s\n", pcap_geterr(PHandle.handle));
        return -1;
    }
    printf("printNextPackage Unknown error.\n");
    return -1;
}

int PrintTCPTraffic(const s8* filter)
{
    int res;
    struct pcap_pkthdr *header;
    const u8 *pkt_data;
    if((1 != PHandle.avaliable) || (PHandle.handle == NULL))
    {
        printf("PHandle is not a live handle.\n");
        return -1;
    }
    if(strlen(filter) > 2)
    {
        if((filter[0] != 't') || (filter[1] != 'c') || (filter[2] != 'p'))
        {
            printf("Illegal filter for printTCPTraffic.\n");
            return -1;
        }
    }
    else
    {
        printf("Illegal filter for printTCPTraffic.\n");
        return -1;
    }
    
    if(1 != SetFilter(filter))
    {
        printf("Illegal filter for printTCPTraffic.\n");
        return -1;
    }

    while(0 <= (res = pcap_next_ex(PHandle.handle, &header, &pkt_data)))
    {
        if(res == 0)
        {
            continue; //time out
        }
        if(res > 0)
        {
            struct tm *ltime;
            time_t local_tv_sec;
            char timestr[16];
            local_tv_sec = header->ts.tv_sec;
            ltime=localtime(&local_tv_sec);
            strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);		
            /* print pkt timestamp and pkt len */
            printf("%s,%.6ld len:%d ", timestr, header->ts.tv_usec, header->len);

            //print ip and port
            IP_HEADER *ih;
            TCP_HEADER *tcph;
            ih = (IP_HEADER *) (pkt_data + sizeof(ETH_HEADER));
            int ip_len = (ih->ver_iphl & 0xF) * 4;
            tcph = (TCP_HEADER *)((u8 *)ih + ip_len);

            u16 sport = tcph->sport;
            u16 dport = tcph->dport;
            sport = ((sport & 0x00FF) << 8) + ((sport & 0xFF00) >> 8); 
            dport = ((dport & 0x00FF) << 8) + ((dport & 0xFF00) >> 8);
            /* print ip addresses and udp ports */
            printf("%d.%d.%d.%d:%d --> %d.%d.%d.%d:%d\n",
                ih->ip_src.byte1,
                ih->ip_src.byte2,
                ih->ip_src.byte3,
                ih->ip_src.byte4,
                sport,
                ih->ip_dst.byte1,
                ih->ip_dst.byte2,
                ih->ip_dst.byte3,
                ih->ip_dst.byte4,
                dport);
            /* Print the packet */
            for (int i=1; (i < header->caplen + 1 ) ; i++)
            {
                printf("%.2x ", pkt_data[i-1]);
                if ( (i % LINE_LEN) == 0) printf("\n");
            }
            printf("\n\n");	
        }
    }
    if(res == -1)
    {
        printf("Error reading the packets: %s\n", pcap_geterr(PHandle.handle));
        return -1;
    }
    printf("printNextPackage Unknown error.\n");
    return -1;
}