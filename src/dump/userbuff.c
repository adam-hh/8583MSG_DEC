#include "8583dump.h"

UserBuff *Usbf = NULL;
UserBuff* InitUserBuff(size_t sz)
{
    UserBuff *ub;
    if((ub = (UserBuff *)malloc(sizeof(UserBuff))) == NULL)
        return NULL;
    if((ub->w = (TcpDataBlock *)malloc(sizeof(TcpDataBlock) * sz)) == NULL)
        return NULL;
    ub->r = ub->w;
    ub->buffStart = ub->w;
    ub->buffSize = sz;

    memset(ub->buffStart, 0, sizeof(TcpDataBlock) * sz);
    return ub;
}

extern int WriteToUserBuff(UserBuff *ubf, TcpDataBlock *blk, size_t dataLen)
{
    if(ubf->w->synFlag == 1)
    {
        fprintf(stderr, "UserBuff full, something wrong with reading, captured data may lost!\n");
        return 0; // UserBuff full
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

int ReadFromUserBuff(UserBuff *ubf, TcpDataBlock* blk)
{
    if(ubf->r->synFlag == 0)
        return 0; //UserBuff empty
    memset(blk, 0, sizeof(TcpDataBlock));
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
    //release the TcpDataBlock after read, so that this block can be write again.
    free(ubf->r->data);
    memset(ubf->r, 0, sizeof(TcpDataBlock));

    ubf->r->synFlag = 0;
    (ubf->r)++;
    if(((ubf->r) - ubf->buffStart) == ubf->buffSize)
        (ubf->r) = ubf->buffStart;
    return 1;
}

int ReleaseUserBuff(UserBuff* ub)
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