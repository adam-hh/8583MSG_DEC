#include "8583dump.h"
#include "com_thaiddd_javaguiex_core_Dump.h"

JNIEnv *lenv = NULL;
extern char *iptos(u_long in);
extern char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen);
extern void callbackPrintNextPackage(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);
extern void callbackWriteToBuff(u8 *param, const struct pcap_pkthdr *header, const u8 *pkt_data);
static pcap_if_t *alldevs; //note, this pcap_if_t heap table inited in devList, should be released in openDev
#define ADDRESSMAXSIZE 25 //address c-string max length
#define ADDRESSMAXROWNUM 25 //address max row number,just experience

classTcpDataBlock classT = {0};
classNICDevice classN = {0};
classString classS = {0};
int getClassInfo()
{
  if(lenv == NULL)
    return 0;
  classT.classTcpDataBlock = (*lenv)->FindClass(lenv, "com/thaiddd/javaguiex/core/TcpDataBlock");CHECKEXPJ(lenv)
  classT.extraInfo =(*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "extraInfo", "[B");
  classT.ipSrc = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "ipSrc", "[B");
  classT.ipDst = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "ipDst", "[B");
  classT.portSrc = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "portSrc", "S");
  classT.portDst = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "portDst", "S");
  classT.dataLen = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "dataLen", "J");
  classT.data = (*lenv)->GetFieldID(lenv, classT.classTcpDataBlock, "data", "[B");

  classN.classNICDevice = (*lenv)->FindClass(lenv, "com/thaiddd/javaguiex/core/NICDevice");CHECKEXPJ(lenv)
  classN.name = (*lenv)->GetFieldID(lenv, classN.classNICDevice, "name", "Ljava/lang/String;");
  classN.description = (*lenv)->GetFieldID(lenv, classN.classNICDevice, "description", "Ljava/lang/String;");
  classN.addresses = (*lenv)->GetFieldID(lenv, classN.classNICDevice, "addresses", "[Ljava/lang/String;");
  classN.flags = (*lenv)->GetFieldID(lenv, classN.classNICDevice, "flags", "Ljava/lang/String;");
  classN.constructor = (*lenv)->GetMethodID(lenv, classN.classNICDevice, "<init>", "()V");

  classS.classString = (*lenv)->FindClass(lenv, "java/lang/String");CHECKEXPJ(lenv)

  void *pst;
  int i;
  for(i = 0, pst = &classT; i<(sizeof(classTcpDataBlock) / sizeof(pst)); i++)
  {
    if(!(long long)((long long *)pst+i))
      return -1;
  }
  for(i = 0, pst = &classN; i<(sizeof(classNICDevice) / sizeof(pst)); i++)
  {
    if(!(long long)((long long *)pst+i))
      return -1;
  }
  for(i = 0, pst = &classS; i<(sizeof(classString) / sizeof(pst)); i++)
  {
    if(!(long long)((long long *)pst+i))
      return -1;
  }
  return 1;
}

JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_devList
  (JNIEnv *env, jclass thisclass, jobjectArray devList)
{
    setbuf(stdout, NULL);
	  setbuf(stderr, NULL);
	  setbuf(stdin, NULL);
    lenv = env; //init lenv
    int rlt = getClassInfo();
    if(-1 == rlt)
    {
      fprintf(stderr, "fatal error, get classInfo failed.\n");
      return -1;
    }
    if(!rlt)
      return -1;
    #ifdef WIN32
	  /* Load Npcap and its functions. */
	  if (!LoadNpcapDlls())
	  {
		  fprintf(stderr, "Couldn't load Npcap\n");
		  return -1;
	  }    
    #endif
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
      return 0;
    }
    //All classes and length and constructor method information from parameter devList
    jclass classNICDevice = classN.classNICDevice;
    jclass classString = classS.classString;
    jsize len = (*env)->GetArrayLength(env, devList);
    jmethodID constructorNICDevice = classN.constructor;

    //All fields of class NICDevice, need to be set here    
    jfieldID fieldName = classN.name;
    jfieldID fieldDescription = classN.description;
    jfieldID fieldFlags = classN.flags;    
    jfieldID fieldAddress = classN.addresses;    

    int i = 0;  
    for(p = alldevs; p; p = p->next)
    {
      jobject ob = (*env)->NewObject(env, classNICDevice, constructorNICDevice);         
      
      (*env)->SetObjectField(env, ob, fieldName, (*env)->NewStringUTF(env, p->name));
      if(p->description)
        (*env)->SetObjectField(env, ob, fieldDescription, (*env)->NewStringUTF(env, p->description));
      s8 flags[ADDRESSMAXSIZE] = {0};
      sprintf(flags, "loopback: %s", (p->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");
      (*env)->SetObjectField(env, ob, fieldFlags, (*env)->NewStringUTF(env, flags));

      pcap_addr_t *a;
      char ip6str[128] = {0};      
      jobjectArray addressArray = (*env)->NewObjectArray(env, ADDRESSMAXROWNUM, classString, 0);
      int j = 0;
      /* IP addresses */
      for(a=p->addresses;a;a=a->next) 
      {
        char af[ADDRESSMAXSIZE] = {0};        
        sprintf(af, "Address Family: #%d",a->addr->sa_family);
        (*env)->SetObjectArrayElement(env, addressArray, j, (*env)->NewStringUTF(env, af));
        switch(a->addr->sa_family)
        {
          case AF_INET:
            (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, "Address Family Name: AF_INET"));            
            if (a->addr)
            {
              char ad[ADDRESSMAXSIZE] = {0};                
              sprintf(ad, "Address: %s",iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
              (*env)->SetObjectArrayElement(env,addressArray, ++j, (*env)->NewStringUTF(env, ad));
            }              
            if (a->netmask)
            {
              char nm[ADDRESSMAXSIZE] = {0};
              sprintf(nm, "Netmask: %s",iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
              (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, nm));
            }            
            if (a->broadaddr)
            {
              char ba[ADDRESSMAXSIZE] = {0};
              sprintf(ba, "Broadcast Address: %s",iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
              (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, ba));
            }            
            if (a->dstaddr)
            {
              char da[ADDRESSMAXSIZE] = {0};
              sprintf(da, "Destination Address: %s",iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
              (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, da));
            }            
          break;
	        case AF_INET6:
            (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, "Address Family Name: AF_INET6"));
#ifndef __MINGW32__ /* Cygnus doesn't have IPv6 */
            if (a->addr)
            {
              char ad[128] = {0};
              sprintf(ad, "address: %s", ip6tos(a->addr, ip6str, sizeof(ip6str)));
              (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, ad));
            }          
#endif
		        break;
	        default:
            (*env)->SetObjectArrayElement(env, addressArray, ++j, (*env)->NewStringUTF(env, "Address Family Name: Unkown"));            
            break;
        }        
        if(j++ > (ADDRESSMAXROWNUM - 7))
        {
          fprintf(stderr, "address row numbers more than ADDRESSMAXROWNUM on this device.\n");
          return 0; // addressArray too short
        }          
      }
      (*env)->SetObjectField(env, ob, fieldAddress, addressArray);      
      (*env)->SetObjectArrayElement(env, devList, i, ob);
      if((i++) == len)
        return 0; // devList too short
  }
  return 1;
}

JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_openDev
  (JNIEnv *env, jclass thisClass, jint in)
{
  if(1 == PHandle.avaliable)
    return 1;
  pcap_if_t *p;
  char errbuf[PCAP_ERRBUF_SIZE];
  int i;
  if(alldevs == NULL)
    {
      fprintf(stderr, "Found 0 NIC device, check whether the Npcap driver was installed correctly.\n");
      return 0;
    }
  for(p = alldevs, i = 0; p; p = p->next)
    i++;
  if(in < 1 || in > i)
    {
      fprintf(stderr, "index %ld outof range 1 - %d.\n", in, i);
      return 0;
    }
  /* Jump to the selected adapter */
	for (p=alldevs, i=0; i< in-1 ;p=p->next, i++);
  if ((PHandle.handle = pcap_open_live(p->name,	// name of the device
		65536,							// portion of the packet to capture. 
										// 65536 grants that the whole packet will be captured on all the MACs.
		1,								// promiscuous mode (nonzero means promiscuous)
		1000,							// read timeout
		errbuf							// error buffer
		)) == NULL)
	{
		fprintf(stderr,"\nError opening adapter\n");
    PHandle.avaliable = -1;
		return -1;
	}
  PHandle.avaliable = 1;
  pcap_freealldevs(alldevs); // free alldevs
  return 1;
}

JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_setFilter
  (JNIEnv *env, jclass thisClass, jstring flt)
{
  if(PHandle.avaliable != 1)
    return -1;
  const s8 *filter = (*env)->GetStringUTFChars(env, flt, NULL);
  if(strlen(filter) == 0)
    return -1;
  int rlt = setFilter(filter);
  (*env)->ReleaseStringUTFChars(env, flt, filter);
  return rlt;
}

userBuff *usbf = NULL;
JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_loop
  (JNIEnv *env, jclass thisClass, jint cnt)
{ 
  if(PHandle.avaliable != 1)
    return -1;
  if(usbf != NULL)
    releaseUserBuff(usbf);
	if((usbf = initUserBuff(100)) == NULL)
  {
    fprintf(stderr, "initUserBuff(100) failed.\n");
    return -1;
  }	
  if(PHandle.avaliable)
	  return loop(cnt, callbackWriteToBuff, (u8*)usbf);
  return -1;
}

JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_stoploop
  (JNIEnv *env, jclass thisclass)
{
  if(PHandle.avaliable != 1)
    return -1;
  pcap_breakloop(PHandle.handle);
  return 1;
}

JNIEXPORT jint JNICALL Java_com_thaiddd_javaguiex_core_Dump_readFromUserBuff
  (JNIEnv *env, jclass thisClass, jobject ob)
{
  tcpDataBlock tdbk = {0};
  if(usbf == NULL)
    return -1;
  int rlt = readFromUserBuff(usbf, &tdbk);
  if(rlt ==  1)
		{
      jfieldID fieldExtraInfo = classT.extraInfo;
      jfieldID fieldIpSrc = classT.ipSrc;
      jfieldID fieldIpDst = classT.ipDst;
      jfieldID fieldPortSrc = classT.portSrc;
      jfieldID fieldPortDst = classT.portDst;
      jfieldID fieldDataLen = classT.dataLen;
      jfieldID fieldData = classT.data;

      jbyteArray jbaExa = (*env)->GetObjectField(env, ob, fieldExtraInfo);
      (*env)->SetByteArrayRegion(env, jbaExa, 0, strlen(tdbk.extraInfo), (jbyte *)tdbk.extraInfo);
      // jbyte *j = (*env)->GetByteArrayElements(env, jbaExa, NULL);
      // memset(j, 0, (*env)->GetArrayLength(jba));
      // memcpy(j, tdbk.data, strlen(tdbk.data));

      jbyteArray jbaIpSrc = (*env)->GetObjectField(env, ob, fieldIpSrc);
      (*env)->SetByteArrayRegion(env, jbaIpSrc, 0, sizeof(tdbk.ipSrc), (jbyte *)&(tdbk.ipSrc));CHECKEXPJ(env)
      jbyteArray jbaIpDst = (*env)->GetObjectField(env, ob, fieldIpDst);
      (*env)->SetByteArrayRegion(env, jbaIpDst, 0, sizeof(tdbk.ipDst), (jbyte *)&(tdbk.ipDst));CHECKEXPJ(env)
     
      (*env)->SetShortField(env, ob, fieldPortSrc, tdbk.portSrc);
      (*env)->SetShortField(env, ob, fieldPortDst, tdbk.portDst);
      (*env)->SetLongField(env, ob, fieldDataLen, tdbk.dataLen);

      jbyteArray jbaData = (*env)->NewByteArray(env, tdbk.dataLen);
      (*env)->SetByteArrayRegion(env, jbaData, 0, tdbk.dataLen, (jbyte *)tdbk.data);CHECKEXPJ(env)
      (*env)->SetObjectField(env, ob, fieldData, jbaData);      

			free(tdbk.data);
			//memset(&tdbk, 0, sizeof(tcpDataBlock));
      return rlt;
		}
		return rlt;
}