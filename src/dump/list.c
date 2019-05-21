#include "8583dump.h"

void ifprint(pcap_if_t *d);
char *iptos(u_long in);
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen);
int PrintDevList()
{
    pcap_if_t *alldevs;
    pcap_if_t *p;
    char errbuf[PCAP_ERRBUF_SIZE + 1];
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return -1;
    }
    if(alldevs == NULL)
    {
        fprintf(stderr, "Found 0 NIC device, check whether the Npcap driver was installed correctly.\n");
        return -1;
    }
    for(p = alldevs; p; p = p->next)
    {
        ifprint(p);
    }

    pcap_freealldevs(alldevs);
    return 1;
}

/* Print all the available information on the given interface */
void ifprint(pcap_if_t *d)
{
    pcap_addr_t *a;
    char ip6str[128] = {0};

    /* Name */
    printf("%s\n",d->name);

    /* Description */
    if (d->description)
    printf("\tDescription: %s\n",d->description);

    /* check flags on Loopback Address, Up, Running */
    printf("\tLoopback: %s\n",(d->flags & PCAP_IF_LOOPBACK)?"yes":"no");
    printf("\tUp: %s\n", (d->flags & PCAP_IF_UP) ? "yes" : "no");
    printf("\tRunning: %s\n", (d->flags & PCAP_IF_RUNNING) ? "yes" : "no");

    /* IP addresses */
    for(a=d->addresses;a;a=a->next) {
        printf("\tAddress Family: #%d\n",a->addr->sa_family);

        switch(a->addr->sa_family)
        {
        case AF_INET:
        printf("\tAddress Family Name: AF_INET\n");
        if (a->addr)
            printf("\tAddress: %s\n",iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
        if (a->netmask)
            printf("\tNetmask: %s\n",iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
        if (a->broadaddr)
            printf("\tBroadcast Address: %s\n",iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
        if (a->dstaddr)
            printf("\tDestination Address: %s\n",iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
        break;

        case AF_INET6:
        printf("\tAddress Family Name: AF_INET6\n");
#ifndef __MINGW32__ /* Cygnus doesn't have IPv6 */
        if (a->addr)
            printf("\tAddress: %s\n", ip6tos(a->addr, ip6str, sizeof(ip6str)));
#endif
        break;

        default:
        printf("\tAddress Family Name: Unknown\n");
        break;
        }
    }
    printf("\n");
}

/* From tcptraceroute, convert a numeric IP address to a string */
#define IPTOSBUFFERS    12
char *iptos(u_long in)
{
    static char output[IPTOSBUFFERS][3*4+3+1]; //nice static, so that stack object can be reference outof this function.
    static short which;
    u_char *p;

    p = (u_char *)&in;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return output[which];
}

#ifndef __MINGW32__ /* Cygnus doesn't have IPv6 */
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen)
{
    socklen_t sockaddrlen;

    #ifdef WIN32
    sockaddrlen = sizeof(struct sockaddr_in6);
    #else
    sockaddrlen = sizeof(struct sockaddr_storage);
    #endif


    if(getnameinfo(sockaddr, 
        sockaddrlen, 
        address, 
        addrlen, 
        NULL, 
        0, 
        NI_NUMERICHOST) != 0) address = NULL;

    return address;
}
#endif /* __MINGW32__ */

int OpenDev()
{
    char errbuf[PCAP_ERRBUF_SIZE + 1];
    if(!PrintDevList())
    {
        printf("No device found on you system.\n");
        return -1;
    }
    printf("Keyin a device name.\n"
        "Example:\n"
        "\"\\Device\\NPF_{DEF8ED50-DD41-483F-AA19-363C874D714E}\"\n\n"
    );
    s8 iname[100] = {0};
    fflush(stdout);
    gets(iname);

    /* Open the adapter */
    if ((PHandle.handle = pcap_open_live(iname,// name of the device
        65536,                          // portion of the packet to capture.
                                        // 65536 grants that the whole packet will be captured on all the MACs.
        1,                              // promiscuous mode (nonzero means promiscuous)
        1000,                           // read timeout
        errbuf                          // error buffer
        )) == NULL)
    {
        fprintf(stderr,"\nError opening adapter\n");
        PHandle.avaliable = -1;
        return -1;
    }
    PHandle.avaliable = 1;
    return 1;
}

int SetFilter(const s8* filter)
{
    struct bpf_program fcode;
    u32 netMask = 0xFFFFFF;
    if(filter == NULL)
        return -1;
    //compile the filter
    if(pcap_compile(PHandle.handle, &fcode, filter, 1, netMask) < 0)
    {
        fprintf(stderr, "\nError compiling filter: wrong syntax.\n");
        return -1;
    }
    //set the filter
    if(pcap_setfilter(PHandle.handle, &fcode) < 0)
    {
        fprintf(stderr, "\nError setting the filter.\n");
        return -1;
    }
    return 1;
}