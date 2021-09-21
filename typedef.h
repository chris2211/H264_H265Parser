#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <string>
struct SRTPInfo
{
    bool bMark;
    int iPayloadType;
    int iSeq;
    int iUDPSrcPort;
    int iUDPDstPort;
    std::string strSrcIP;
    std::string strDstIP;
    std::string strSSRC;
};

enum enNaluType
{
    enNaluType_UnKnow = -1,
    //��֡��һ��rtp���оʹ���һ֡����
    enNaluType_Single = 0,
    //FUA�Ŀ�ͷ
    enNaluType_FUAStart = 1,
    //FUA����֡
    enNaluType_FUA = 2,
    //FUA�Ľ���
    enNaluType_FUAEnd = 3
};

enum enFUAState
{
    enFUAState_Init = 0,
    enFUAState_Start,
    enFUAState_Data,
    enFUAState_End,
    enFUAState_Terminate
};

struct SH265PacketParams
{
    SH265PacketParams()
    {
        bMark = false;
        type = enNaluType_UnKnow;
        memset(H265NalHeader, 0, 2);
    }

    bool bMark;
    enNaluType type;
    //Nal��Ԫ��ͷһ���ֽ������ж�֡����
    unsigned char H265NalHeader[2];
};

struct SH264PacketParams
{
    SH264PacketParams()
    {
        bMark = false;
        type = enNaluType_UnKnow;
    }

    bool bMark;
    enNaluType type;
    //Nal��Ԫ��ͷһ���ֽ������ж�֡����
    unsigned char H264NalHeader;
};

#endif