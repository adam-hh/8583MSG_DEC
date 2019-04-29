/*Author: adamH.H Contact:409520259@qq.com
 *Date:2019.4.3
 *Description:
 *This header file used for:
 *1.declare functions which wrapped from npcap-sdk-1.01 for message dump.
 *2.declare some clobal fields.
 *3.declare other functions for message dump.
 *4.include the headers from npcap-sdk-1.01 and necessary standard and OS dependent socket headers.
 *All modules who want to implement or involve these fields should include this header
 */
#pragma once
#ifndef DUMP8583
#define DUMP8583

#include <stdlib.h>
#include <stdio.h>
#include "pcap.h"
#include "protocal_head.h"
#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
#else
	#include <winsock.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _MSC_VER
	/*
	 * we do not want the warnings about the old deprecated and unsecure CRT functions
	 * since these examples can be compiled under *nix as well
	 */
#define _CRT_SECURE_NO_WARNINGS
#endif
#define EXTRAINFOSIZE 30


//a global pcap_t handle, for the whole life cycle of this program.
//this should be an unique handle for all components.
typedef struct{
	pcap_t *handle;		
	s32 avaliable;	
}PcapHandle;
extern PcapHandle PHandle;
/*Function:LoadNpcapDlls
 *Description:works on Windows OS, testing whether the Npcap driver installed or not
 *Input: no
 *Output:no
 *Return:test result true or false
 */
#ifdef WIN32
#include <tchar.h>
#endif
extern int LoadNpcapDll();

/*Function:printDevList
 *Description:print the device list to stdout
 *Input:no
 *Output:no
 *Return:1 on sucess, -1 on fail
 */
extern int printDevList();

/*Function:openDev
 *Description:open a device from the Dev list
 *Input:no
 *Output:no
 *Return:1 on sucess, -1 on fail
 */
extern int openDev();

/*Function:setFilter
 *Description:compile then set a filter on PHandle
 *Input: a c-string filter
 *Output:no
 *Return: 1 if sucess, -1 if failed
 */
extern int setFilter(const s8*);

/*Function:printNextPackege
 *Description:print one package from PHandle, PHandle must be actived
 *Input:no
 *Output:no
 *Return: 1 if without problem; 0 if timeout(1000ms); -1 if error occurred
 */
extern int printNextPackage();

/*Function:printTCPTraffic
 *Description:set a "tcp" filter on PHandle, print the message with extra description from the tcp/ip headres cycling.
 *NOTE: printTCPTraffic will do cycle trying to print avaliable data from PHandle. So it should be put to a seperate thread.
 *Input:filter c-string
 *Output:no
 *Return: 1 if without problem; -1 if error occurred
 */
extern int printTCPTraffic(const s8*);

/*Function:loop
 *Description:loop the traffic datas of PHandle, 
 *Input:cnt, callback, user
 *Output:no
 *Return: 1 if without problem; -1 if error occurred
 *Other:  the callback function is
 *typedef void (*pcap_handler)(u_char *, const struct pcap_pkthdr *,
	const u_char *);
 *user* passed to u_char*, pcap_pkthdr* point to the current pcap_pkthdr, u_char*  point to the data memory first byte address
 */
extern int loop(int cnt, pcap_handler callback, u8 *user);
extern void callbackPrintNextPackage(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);
extern void callbackWriteToBuff(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);

//declear a struct for java native functions
//the 8583 message datas are orgnized with this struct.
//then the datas can be read via java reference on the java GUI program. 
typedef struct{
	s8 **ip_tcp_info;
	u8 *msgData;
	size_t msgDataLen;
}msgHdr;

//tcp data block
typedef struct{
	s8 extraInfo[EXTRAINFOSIZE];
	ip_address ipSrc;
	ip_address ipDst;
	u16 portSrc;
	u16 portDst;
	u8 *data;
	size_t dataLen;
	u32 synFlag; //a flag for parallel enviorment read/write sync
}tcpDataBlock;

//user buff
typedef struct{
	tcpDataBlock *w;
	tcpDataBlock *r;
	tcpDataBlock *buffStart;
	size_t buffSize;
}userBuff;
extern userBuff *usbf;

/*Function: initUserBuff
 *Description:create a userBuff struct and init it
 *Input:sz is the size
 *Output:no
 *Return: pst to the inited userBuff
 */
extern userBuff* initUserBuff(size_t sz);

/*Function: writeToUserBuff
 *Description:fill one tcpDataBlock into the ub
 *Input:ub is the userBuff pst, * is the tcpDataBlock pst to be filled. dataLen is the data's length in the tcpDataBlock
 *Output:
 *Return: 1 if sucess, 0 if userBuff is full, -1 if error occured
 */
extern int writeToUserBuff(userBuff *ub, tcpDataBlock *, size_t dataLen);

/*Function: readFromUserBuff
 *Description:read one tcpDataBlock from the ub to *
 *Input:ub is the userBuff pst
 *Output:tcpDataBlock* is for data output
 *Return: 1 if sucess, 0 if userBuff is empty, -1 if error occured
 */
extern int readFromUserBuff(userBuff *ub, tcpDataBlock*);

//release the user buffer
extern int releaseUserBuff(userBuff *);

#ifdef __cplugsplus
}
#endif

#endif