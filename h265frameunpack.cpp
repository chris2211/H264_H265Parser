#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include "h265frameunpack.h"

#define MAX_FRAME_SIZE 1024 * 1024

CH265FrameUnpack::CH265FrameUnpack()
{
    m_iEncodeFrameLen = 0;
    m_iEncodeFrameMax = MAX_FRAME_SIZE;

    m_pEncodeFrame = (unsigned char*)malloc(MAX_FRAME_SIZE);
    memset(m_pEncodeFrame, 0, MAX_FRAME_SIZE);
}

CH265FrameUnpack::~CH265FrameUnpack()
{
    m_iEncodeFrameLen = 0;
    if (m_pEncodeFrame)
    {
        free(m_pEncodeFrame);
        m_pEncodeFrame = NULL;
    }
}

void CH265FrameUnpack::ResetFramePool()
{
    m_iEncodeFrameMax = MAX_FRAME_SIZE;
    m_iEncodeFrameLen = 0;
    memset(m_pEncodeFrame, 0, MAX_FRAME_SIZE);
}

void CH265FrameUnpack::AddData(unsigned char* pData, int iDataLen, unsigned char* pNaluHeader, bool bAddHeader,bool isMark)
{
    int iHeaderLen = 0;
    if (bAddHeader)
    {
        //NalU的起始四字节加上NALU头的两字节
        iHeaderLen = 6;
    }

    unsigned char *pCurrentPosiotionInFrame = m_pEncodeFrame + m_iEncodeFrameLen;
    while (m_iEncodeFrameLen + iDataLen + iHeaderLen > m_iEncodeFrameMax)
    {
        m_iEncodeFrameMax += MAX_FRAME_SIZE;
        m_pEncodeFrame = (unsigned char*)realloc(m_pEncodeFrame, m_iEncodeFrameMax);
        pCurrentPosiotionInFrame = m_pEncodeFrame + m_iEncodeFrameLen;
    }

    if (bAddHeader)
    {
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 0;
        *pCurrentPosiotionInFrame++ = 1;

        //设置NALU的头
        memcpy(pCurrentPosiotionInFrame, pNaluHeader, 2);

        pCurrentPosiotionInFrame += 2;
    }

    memcpy(pCurrentPosiotionInFrame, pData, iDataLen);
    m_iEncodeFrameLen += iDataLen + iHeaderLen;
}

int CH265FrameUnpack::SetFrameData(unsigned char* pData, int iSize, SH265PacketParams& params)
{//根据包类型判断是否加入头
    if (enNaluType_Single == params.type)
    {
        AddData(pData, iSize,params.H265NalHeader, true);
    }
    else if (enNaluType_FUAStart == params.type)
    {
        AddData(pData, iSize,params.H265NalHeader, true);
    }
    else if (enNaluType_FUA == params.type || enNaluType_FUAEnd == params.type)
    {
        AddData(pData, iSize, NULL, false);
    }

    return 0;
}