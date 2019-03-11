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


extern int BCDDecode(const u8 *src, char *des, size_t l);
extern int BCDEncode(const char *src, u8 *des, size_t l);
extern unsigned int BCDCal(u8 *src, size_t l, HowNumberAlign align);
extern int Hexview(const u8 *src, char *des, size_t l);
extern int HexviewcharToHex(const char *src, u8 *des, size_t l);
extern unsigned int CalSmalMemoryPieceVal(const u8 *, size_t l);

