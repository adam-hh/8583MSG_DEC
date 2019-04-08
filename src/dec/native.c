/*---------------------
Author HuHui(Adam)
implement native functions from com_thaiddd_javeguiex.Native.hS
---------------------*/
#include "com_thaiddd_javaguiex_core_NativeC.h"
#include "8583.h"
#define STDOUTREDERECT
/*
 * Class:     com_thaiddd_javaguiex_NativeC
 * Method:    nativeDecStr
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_thaiddd_javaguiex_core_NativeC_nativeDecStr
  (JNIEnv *env, jclass thisObj, jstring inputJstr)
  {
    #ifdef STDOUTREDERECT
    remove("c_console");
    freopen("c_console", "a", stdout);
    #endif
    //setbuf(stdout, NULL);
    const s8 *msgIn = (*env)->GetStringUTFChars(env, inputJstr, NULL);
    if(0 == strlen(msgIn))
    {
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
      return NULL;
    }
      
    size_t msgLen = strlen(msgIn) / 2;
    u8 *msg = (u8*)malloc(msgLen);
    memset(msg, 0, msgLen);

    if(!HexviewcharToHex(msgIn, msg, msgLen))
    {
      printf("illegal hexview string!\n");
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
      return NULL;
    }
    (*env)->ReleaseStringUTFChars(env, inputJstr, msgIn);

    if(2 > msgLen)
    {
      #ifdef STDOUTREDERECT
      printf("message lengh error\n");
      fclose(stdout);
      #endif
	    return NULL;
    }
    if(CalSmalMemoryPieceVal(msg, 2) != (msgLen - 2))
	  {
		  printf("message lengh check failed.\nexpected length is %u, actual string length of the message is %lu\n", CalSmalMemoryPieceVal(msg, 2) + 2, msgLen);
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
		  return NULL;
	  } 
    Decoder MsgDecoder = {0};
	  int rlt = DecodeMemMsg(msg, &MsgDecoder, msgLen);
	  if(FAIL == rlt)
	  {
		  printf("message decode failed.\n");
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
		  return NULL;
		}
	  else if(FATAL_ERROR == rlt)
		{
		  printf("fatal error: memory vialation.\n");	
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
		  return NULL;
		}
    
    
    jclass jstringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray jobjArray = (*env)->NewObjectArray(env, 68, jstringClass, NULL);

    size_t fieldLen_l = (MsgDecoder.FieldLen_l) * 2 + 1;
    s8 *fieldLen = (s8*)malloc(fieldLen_l);
    memset(fieldLen, 0, fieldLen_l);
    Hexview(msg + MsgDecoder.FieldLen, fieldLen, MsgDecoder.FieldLen_l);
    jstring jfieldLen = (*env)->NewStringUTF(env, fieldLen);
    (*env)->SetObjectArrayElement(env, jobjArray, 0, jfieldLen);
    free(fieldLen);

    size_t fieldTPDU_l = (MsgDecoder.FieldTPDU_l) * 2 + 1;
    s8 *fieldTPDU = (s8*)malloc(fieldTPDU_l);
    memset(fieldTPDU, 0, fieldTPDU_l);
    Hexview(msg + MsgDecoder.FieldTPDU, fieldTPDU, MsgDecoder.FieldTPDU_l);
    jstring jfieldTPDU = (*env)->NewStringUTF(env, fieldTPDU);
    (*env)->SetObjectArrayElement(env, jobjArray, 1, jfieldTPDU);
    free(fieldTPDU);
    
    size_t fieldMsghead_l = (MsgDecoder.FieldMsghead_l) *2 + 1;
    s8 *fieldMsghead = (s8*)malloc(fieldMsghead_l);
    memset(fieldMsghead, 0, fieldMsghead_l);
    Hexview(msg + MsgDecoder.FieldMsghead, fieldMsghead, MsgDecoder.FieldMsghead_l);
    jstring jfieldMsghead = (*env)->NewStringUTF(env, fieldMsghead);
    (*env)->SetObjectArrayElement(env, jobjArray, 2, jfieldMsghead);
    free(fieldMsghead);

    #define F(x,y) \
	  size_t field##x##_l = (MsgDecoder.Field##x##_l) * 2 + 1;\
	  if(field##x##_l != 1) {\
      s8 *field##x = (s8*)malloc(field##x##_l);memset(field##x, 0, field##x##_l);Hexview(msg + MsgDecoder.Field##x, field##x, MsgDecoder.Field##x##_l);\
      jstring jfield##x = (*env)->NewStringUTF(env, field##x);(*env)->SetObjectArrayElement(env, jobjArray, y, jfield##x);\
      free(field##x);\
    }
    
    F(0,3) F(1,4) F(2,5) F(3,6) F(4,7) F(5,8) F(6,9) F(7,10) F(8,11) F(9,12) F(10,13)
    F(11,14) F(12,15) F(13,16) F(14,17) F(15,18) F(16,19) F(17,20) F(18,21) F(19,22) F(20,23)
    F(21,24) F(22,25) F(23,26) F(24,27) F(25,28) F(26,29) F(27,30) F(28,31) F(29,32) F(30,33)
    F(31,34) F(32,35) F(33,36) F(34,37) F(35,38) F(36,39) F(37,40) F(38,41) F(39,42) F(40,43)
    F(41,44) F(42,45) F(43,46) F(44,47) F(45,48) F(46,49) F(47,50) F(48,51) F(49,52) F(50,53)
    F(51,54) F(52,55) F(53,56) F(54,57) F(55,58) F(56,59) F(57,60) F(58,61) F(59,62) F(60,63)
    F(61,64) F(62,65) F(63,66) F(64,67)
    #undef F 
  
    free(msg);
    #ifdef STDOUTREDERECT
    fclose(stdout);
    #endif
	  return jobjArray;
  }

  /*
 * Class:     com_thaiddd_javaguiex_NativeC
 * Method:    nativeDecBin
 * Signature: ([B)[[B
 */
JNIEXPORT jobjectArray JNICALL Java_com_thaiddd_javaguiex_core_NativeC_nativeDecBin
  (JNIEnv *env, jclass thisObj, jbyteArray inputJbyteArray)
  {
    #ifdef STDOUTREDERECT
    remove("c_console");
    freopen("C_console", "a", stdout);
    #endif
	  //setbuf(stdout, NULL);
	  jbyte *inByteArray = (*env)->GetByteArrayElements(env, inputJbyteArray, NULL);
    if(NULL == inByteArray)
    {
      printf("illegal byte arrary input.\n");
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
      return NULL;
    }
    u8 *msg = (u8*)inByteArray;
    jsize msgLen = (*env)->GetArrayLength(env, inputJbyteArray);    

    if(2 > msgLen)
    {
      #ifdef STDOUTREDERECT
      printf("message lengh error\n");
      fclose(stdout);
      #endif
	    return NULL;
    }
    if(CalSmalMemoryPieceVal(msg, 2) != (msgLen - 2)) 
	  {
		  printf("message lengh check failed.\nexpected length is %u, actual string length of the message is %lu\n", CalSmalMemoryPieceVal(msg, 2) + 2, msgLen);
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
		  return NULL;
	  } 
    Decoder MsgDecoder = {0};
	  int rlt = DecodeMemMsg(msg, &MsgDecoder, msgLen);
	  if(FAIL == rlt)
		{
		  printf("message decode failed.\n");
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
			return NULL;
		}
	  else if(FATAL_ERROR == rlt)
		{
			printf("fatal error: memory vialation.\n");	
      #ifdef STDOUTREDERECT
      fclose(stdout);
      #endif
			return NULL;
		}

    jclass jbyteArrayClass = (*env)->FindClass(env, "java/lang/Object");
    jobjectArray jobjArray = (*env)->NewObjectArray(env, 68, jbyteArrayClass, NULL);

    jsize fieldLen_l = MsgDecoder.FieldLen_l;
    jbyteArray fieldLen = (*env)->NewByteArray(env, fieldLen_l);
    (*env)->SetByteArrayRegion(env, fieldLen, 0, fieldLen_l, msg + MsgDecoder.FieldLen);
    (*env)->SetObjectArrayElement(env, jobjArray, 0, fieldLen);

    jsize fieldTPDU_l = MsgDecoder.FieldTPDU_l;
    jbyteArray fieldTPDU = (*env)->NewByteArray(env, fieldTPDU_l);
    (*env)->SetByteArrayRegion(env, fieldTPDU, 0, fieldTPDU_l, msg + MsgDecoder.FieldTPDU);
    (*env)->SetObjectArrayElement(env, jobjArray, 1, fieldTPDU);

    jbyteArray fieldMsghead = (*env)->NewByteArray(env, MsgDecoder.FieldMsghead_l);
    (*env)->SetByteArrayRegion(env, fieldMsghead, 0, MsgDecoder.FieldMsghead_l, msg + MsgDecoder.FieldMsghead);
    (*env)->SetObjectArrayElement(env, jobjArray, 2, fieldMsghead);

    #define F(x,y) \
	  if(MsgDecoder.Field##x##_l != 0) {\
	    jbyteArray field##x = (*env)->NewByteArray(env, MsgDecoder.Field##x##_l);\
      (*env)->SetByteArrayRegion(env, field##x, 0, MsgDecoder.Field##x##_l, msg + MsgDecoder.Field##0);\
	    (*env)->SetObjectArrayElement(env, jobjArray, y, field##x);\
	  }

    F(0,3) F(1,4) F(2,5) F(3,6) F(4,7) F(5,8) F(6,9) F(7,10) F(8,11) F(9,12) F(10,13)
    F(11,14) F(12,15) F(13,16) F(14,17) F(15,18) F(16,19) F(17,20) F(18,21) F(19,22) F(20,23)
    F(21,24) F(22,25) F(23,26) F(24,27) F(25,28) F(26,29) F(27,30) F(28,31) F(29,32) F(30,33)
    F(31,34) F(32,35) F(33,36) F(34,37) F(35,38) F(36,39) F(37,40) F(38,41) F(39,42) F(40,43)
    F(41,44) F(42,45) F(43,46) F(44,47) F(45,48) F(46,49) F(47,50) F(48,51) F(49,52) F(50,53)
    F(51,54) F(52,55) F(53,56) F(54,57) F(55,58) F(56,59) F(57,60) F(58,61) F(59,62) F(60,63)
    F(61,64) F(62,65) F(63,66) F(64,67)
    #undef F
    free(msg);
    #ifdef STDOUTREDERECT
    fclose(stdout);
    #endif
    return jobjArray;
  }
