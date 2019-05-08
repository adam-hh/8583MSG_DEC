#include "8583dump.h"
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

userBuff *usbf = NULL;
userBuff* initUserBuff(size_t sz)
{
    userBuff *ub;
    if((ub = (userBuff *)malloc(sizeof(userBuff))) == NULL)
        return NULL;
    if((ub->w = (tcpDataBlock *)malloc(sizeof(tcpDataBlock) * sz)) == NULL)
        return NULL;
    ub->r = ub->w;
    ub->buffStart = ub->w;
    ub->buffSize = sz;

    memset(ub->buffStart, 0, sizeof(tcpDataBlock) * sz);
    return ub;
}

extern int writeToUserBuff(userBuff *ubf, tcpDataBlock *blk, size_t dataLen)
{
    if(ubf->w->synFlag == 1)
    {
        fprintf(stderr, "userBuff full, something wrong with reading, captured data may lost!\n");
        return 0; // userBuff full
    }        
    ubf->w->ipSrc = blk->ipSrc;
    ubf->w->ipDst = blk->ipDst;
    ubf->w->portSrc = blk->portSrc;
    ubf->w->portDst = blk->portDst;
    ubf->w->dataLen = blk->dataLen;
    memcpy(ubf->w->timestr, blk->timestr, TIMESTRSIZE);
    ubf->w->tv_usec = blk->tv_usec;
    ubf->w->totallen = blk->totallen;

    if((ubf->w->data = (u8 *)malloc(dataLen)) == NULL)
        return -1;
    memcpy(ubf->w->data, blk->data, dataLen);

    ubf->w->synFlag = 1;
    (ubf->w)++;
    if(((ubf->w) - ubf->buffStart) == ubf->buffSize)
        (ubf->w) = ubf->buffStart;
    return 1;
}

extern int readFromUserBuff(userBuff *ubf, tcpDataBlock* blk)
{
    if(ubf->r->synFlag == 0)
        return 0; //userBuff empty
    memset(blk, 0, sizeof(tcpDataBlock));
    blk->ipSrc = ubf->r->ipSrc;
    blk->ipDst = ubf->r->ipDst;
    blk->portSrc = ubf->r->portSrc;
    blk->portDst = ubf->r->portDst;
    blk->dataLen = ubf->r->dataLen;
    memcpy(blk->timestr, ubf->r->timestr, TIMESTRSIZE);
    blk->tv_usec = ubf->r->tv_usec;
    blk->totallen = ubf->r->totallen;

    if((blk->data = (u8 *)malloc(blk->dataLen)) == NULL)
        return -1;
    memcpy(blk->data, ubf->r->data, blk->dataLen);
    //release the tcpDataBlock after read, so that this block can be write again.
    free(ubf->r->data);
    memset(ubf->r, 0, sizeof(tcpDataBlock));

    ubf->r->synFlag = 0;
    (ubf->r)++;
    if(((ubf->r) - ubf->buffStart) == ubf->buffSize)
        (ubf->r) = ubf->buffStart;
    return 1;
}

int releaseUserBuff(userBuff* ub)
{
    for(int i = 0; i<ub->buffSize; i++)
    {
        if(ub->buffStart[i].data)
            free(ub->buffStart[i].data);
    }
    free(ub->buffStart);
    free(ub);
    return 1;
}