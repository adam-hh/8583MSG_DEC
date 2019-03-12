/*--------------
Author Huhui(Adam)
Head file for 8583 project
----------------*/
#define OK 1
#define FAIL 0
#define FATAL_ERROR -1
typedef enum {RIGHT, LEFT, DEFAULT} HowNumberAlign;
typedef unsigned char u8;
typedef unsigned int u32;

/*-----------------------
Decoder to store the field's location
-----------------------*/
#define F(x) u32 Field##x, Field##x_l
typedef struct
{
	u8 *Msgmem = NULL; //pst of message memory head
	size_t Msgmem_l;  //message  memory piece length
	u32 FieldLen, FieldLen_l; // FieldLen is the offset of field "FieldLen" head, Field_l is the length of the field. 
	u32 FieldTPDU, FieldTPDU_l;
	u32 FieldMsghead, FieldMsghead_l;
	u32 Field0, Field0_l; // message type
	u32 Field1, Field1_l; // bitmap
	u32 Field2, Field2_l; // 8583 field2
	u32 Field3, Field3_l;
	u32 Field4, Field4_l;
	u32 Field5, Field5_l;
	F(6);F(7);F(8);F(9);F(10);F(11);F(12);F(13);F(14);F(15);F(16);F(17);F(18);F(19);F(20);
	F(21);F(22);F(23);F(24);F(25);F(26);F(27);F(28);F(29);F(30);F(31);F(32);F(33);F(34);F(35);
	F(36);F(37);F(38);F(39);F(40);F(41);F(42);F(43);F(44);F(45);F(46);F(47);F(48);F(49);F(50);
	F(51);F(52);F(53);F(54);F(55);F(56);F(57);F(58);F(59);F(60);F(61);F(62);F(63);F(64);	
}*PtrDecoder, Decoder;
#undef F(x)

extern int BCDDecode(const u8 *src, char *des, size_t l);
extern int BCDEncode(const char *src, u8 *des, size_t l);
extern unsigned int BCDCal(u8 *src, size_t l, HowNumberAlign align);
extern unsigned int BCDCalInMsgLenField(u8 *src, size_t l, HowNumberAlign align);
extern int Hexview(const u8 *src, char *des, size_t l);
extern int HexviewcharToHex(const char *src, u8 *des, size_t l);
extern unsigned int CalSmalMemoryPieceVal(const u8 *, size_t l);
extern int DecodeMemMsg(const u8*, PtrDecoder, size_t l);
extern int BitMaptest(const int, const u8*, size_t);


