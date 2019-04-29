#ifndef PROTOCAL_HEAD
#define PROTOCAL_HEAD
#include <primitive_type.h>


/*********************************************/     
#define ETHER_ADDR_LEN 6 //NIC physic address length 6 bytes   

/*********************************************/  
//ethernet header struct
/*********************************************/  
typedef struct ether_header  
{  
    u8 ether_dhost[ETHER_ADDR_LEN];  
    u8 ether_shost[ETHER_ADDR_LEN];  
    u16 ether_type;  
}ETH_HEADER;  
 

/* 4 bytes IP address struct */
typedef struct ip_address{
	u8 byte1;
	u8 byte2;
	u8 byte3;
	u8 byte4;
}ip_address;


/* arp header struct */  
typedef struct arphdr  
{  
    u16 ar_hrd;  
    u16 ar_pro;  
    u8 ar_hln;  
    u8 ar_pln;  
    u16 ar_op;  
}ARP_HEADER;  
  
/*********************************************/  
//IP header struct
/*********************************************/
typedef struct iphdr  
{  
    u8 ver_iphl;	//version 4 bits + ip header length 4 bits  
    u8 ip_tos;  	// TOS (type of service)
    u16 ip_len;  	//ip message length
    u16 ip_id;  	//identification
    u16 flag_off;  	//flag 3 bits + fragment offset 13 bits
    u8 ip_ttl;  	//time to live
    u8 ip_protocal; //protocal
    u16 ip_checksum;//checksum crc
    struct ip_address ip_src;	//source ip address 
    struct ip_address ip_dst;  //destination ip address
}IP_HEADER;  
 
/*********************************************/  
//TCP header struct   
/*********************************************/ 
#define TH_FIN 0x01		//mask to FIN flag bit
#define TH_SYN 0x02   	//mask to SYN flag bit
#define TH_RST 0x04   	//mask to RST flag bit
#define TH_PSH 0x08   	//mask to PSH flag bit
#define TH_ACK 0x10   	//mask to ACK flag bit
#define TH_URG 0x20  	//mask to URG flag bit
typedef struct tcphdr   
{  
    u16 sport;	//source port number
    u16 dport;  //destination port number
    u32 seq; 	// sequnce number
    u32 ack;	//ack number  
    u16 off_resv_cflag;  //tcp header length 4 bits + reserved 6 bits + control flags 6 bits     
    u16 window;	//window
    u16 checksum;//checksum
    u16 urp; 	//urgent pst
}TCP_HEADER;  
 
/*********************************************/  
//UDP header struct
/*********************************************/    
typedef struct udphdr   
{  
    u16 sport;	//source port  
    u16 dport;	//destination port
    u16 ulen;  	//length of udp header + data length
    u16 checksum;//checksum  	
}UDP_HEADER;  
#endif