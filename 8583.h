/*--------------
Author Huhui(Adam)
Head file for 8583 project
----------------*/
typedef enum {RIGHT, LEFT, DEFAULT} HowNumberAlign;
typedef unsigned char u8;
typedef unsigned int u32;

extern int BCDDecode(u8*, char*);
extern int BCDEncode(char*, u8*);
extern unsigned int BCDCal(u8*, int, HowNumberAlign);
extern int Hexview(u8*, char*);
extern int HexviewcharToHex(char*, u8*);

