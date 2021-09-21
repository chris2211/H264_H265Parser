#include <stdlib.h>
#include <memory.h>
#include "H264FrameUnpack.h"
 
#define MAX_FRAME_SIZE 1024 * 1024
 
CH264FrameUnpack::CH264FrameUnpack():m_pEncodeFrame(NULL),m_iEncodeFrameLen(0)
{
    m_iEncodeFrameLen = MAX_FRAME_SIZE;
    m_iEncodeFrameMax = MAX_FRAME_SIZE;
    ResetFramePool();
}
 
CH264FrameUnpack::~CH264FrameUnpack()
{
    m_iEncodeFrameLen = 0;
    if (m_pEncodeFrame)
    {
        free(m_pEncodeFrame);
        m_pEncodeFrame = NULL;
    }
}
 
void CH264FrameUnpack::ResetFramePool()
{
    m_iEncodeFrameLen = 0;
    if (m_pEncodeFrame == NULL)
    {
		m_pEncodeFrame = (unsigned char*)malloc(MAX_FRAME_SIZE);
		memset(m_pEncodeFrame,0,MAX_FRAME_SIZE);
    }
}
 
void CH264FrameUnpack::AddData(unsigned char* pData,int iDataLen,unsigned char pNaluHeader,bool bAddHeader)
{
    int iHeaderLen = 0;
    if (bAddHeader)
    {//判断是否是第一个分片，第一个分片是需要添加头的
        //NalU的起始四字节加上NALU头的一个字节
        iHeaderLen = 5;
    }
    
    unsigned char *pCurrentPosiotionInFrame = m_pEncodeFrame + m_iEncodeFrameLen;
    while (m_iEncodeFrameLen + iDataLen + iHeaderLen > m_iEncodeFrameMax)
    {//当缓存不够时则扩大缓存
        m_iEncodeFrameMax += MAX_FRAME_SIZE;
        m_pEncodeFrame = (unsigned char*)realloc(m_pEncodeFrame,m_iEncodeFrameMax);
        pCurrentPosiotionInFrame = m_pEncodeFrame + m_iEncodeFrameLen;
    }
 
    if (bAddHeader)
    {//添加4字节的头标识 0001
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 1;
 
        //设置NALU的头
        memcpy(pCurrentPosiotionInFrame,&pNaluHeader,1);
 
        pCurrentPosiotionInFrame += 1;
    }
    
    //将分片数据组合
    memcpy(pCurrentPosiotionInFrame, pData, iDataLen);
    m_iEncodeFrameLen += iDataLen+ iHeaderLen;
}
 
int CH264FrameUnpack::SetFrameData(unsigned char* pData,int iSize,SH264PacketParams params)
{//根据包类型判断是否加入头
    if (enNaluType_Single == params.type)
    {
        AddData(pData,iSize,params.H264NalHeader,true);
    }
    else if (enNaluType_FUAStart == params.type)
    {
        AddData(pData,iSize,params.H264NalHeader,true);
    }
    else if (enNaluType_FUA == params.type || enNaluType_FUAEnd == params.type)
    {
        AddData(pData,iSize,NULL,false);
    }
 
    return 0;
}