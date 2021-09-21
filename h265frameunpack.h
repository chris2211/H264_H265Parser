#ifndef H265_FRAME_UNPACK_H
#define H265_FRAME_UNPACK_H
#include "typedef.h"

//H265解包类，实现组帧操作
class CH265FrameUnpack
{
public:

    CH265FrameUnpack();
    ~CH265FrameUnpack();

    void ResetFramePool();
    //传入原始视频数据
    int SetFrameData(unsigned char* pData, int iSize, SH265PacketParams& params);
    int GetFrameSize()
    {
        return m_iEncodeFrameLen;
    }

    //获取组成的H264一帧数据
    unsigned char* GetFramePtr()
    {
        return m_pEncodeFrame;
    }

private:
    void AddData(unsigned char* pData, int iDataLen, unsigned char* pNaluHeader, bool bAddHeader, bool isMark = false);
private:
    unsigned char *m_pEncodeFrame;
    unsigned int m_iEncodeFrameLen;
    unsigned int  m_iEncodeFrameMax;
};

#endif