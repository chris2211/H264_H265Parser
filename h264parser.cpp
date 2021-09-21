#include <iostream>
#include "h264parser.h"
#include "h264frameunpack.h"

CH264Parser* CH264Parser::m_pInstance = NULL;

CH264Parser* CH264Parser::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CH264Parser;
    }

    return m_pInstance;
}

void CH264Parser::Release()
{
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

int CH264Parser::Init(int iPayloadType, const std::string& strSSRC, std::string& strOutputFile,
    const std::string& strSrcIP, int iSrcPort, const std::string& strDstIP, int iDstPort)
{
    m_strSrcIP = strSrcIP;
    m_strDstIP = strDstIP;
    m_iSrcPort = iSrcPort;
    m_iDstPort = iDstPort;
    m_strSSRC = strSSRC;
    m_iPayLoadType = iPayloadType;
    m_iCurretnKeyFrame = 0;
    m_pOutputFile = fopen(strOutputFile.c_str(), "wb");
    if (NULL == m_pOutputFile)
    {
        return -1;
    }

    return 0;
}

void CH264Parser::ParserH264(const SRTPInfo& rtpInfo, unsigned char* pData, int iPayloadSize)
{
    if ((rtpInfo.strSSRC != m_strSSRC) || rtpInfo.iPayloadType != m_iPayLoadType ||
        (rtpInfo.iUDPDstPort != m_iDstPort))
    {
        return;
    }


    int iRet = ProcessH264Video(pData, iPayloadSize, rtpInfo.bMark, m_h264Frame);
    if (-1 == iRet)
    {
        return;
    }

    if (rtpInfo.bMark)
    {
        unsigned char* pWirteData = m_h264Frame.GetFramePtr();
        int iDataSize = m_h264Frame.GetFrameSize();

        if (m_pOutputFile)
        {
            fwrite(pWirteData, iDataSize, 1, m_pOutputFile);
        }

        m_h264Frame.ResetFramePool();
    }
}

int CH264Parser::ProcessH264Video(unsigned char* pData, unsigned int iSize, bool isMark, CH264FrameUnpack &h264unpack)
{
    if (NULL == pData)
    {
        return -1;
    }

    unsigned char* pHeaderStart = pData;
    unsigned int PacketSize = iSize;
    // 0x1F的二进制为0001 1111,pHeaderStart[0]为rtp数据的一个字节(FU indicator)为大端序,取出type的值
    int type = pHeaderStart[0] & 0x1F;
    int iPacketSize = iSize;
    if (type == 28)
    {//FU-A
     //pHeaderStart[1]为rtp数据的第二字节(FU header)为大端序,取出S标识
        unsigned char startBit = pHeaderStart[1] >> 7;
        //0x40的二进制为0100 0000,右移6位转换位小端,取出E标识
        unsigned char endBit = (pHeaderStart[1] & 0x40) >> 6;

        SH264PacketParams params;
        params.bMark = isMark;

        if (startBit)
        {//FU-A的起始
            pData +=2 ;
            iPacketSize -= 2;
            params.type = enNaluType_FUAStart;
            //组成nal header
            //Nal header = Forbidden_bit(1位) + nal_reference_bit(2位) + Nal_unit_type(5位)
            params.H264NalHeader = ((*(pData - 1)) & 0xE0) | (pData[0] & 0x1F);
        }
        else
        {// end
            pData += 2;
            iPacketSize -= 2;
            if (endBit)
                params.type = enNaluType_FUAEnd;
            else
                params.type = enNaluType_FUA;

        }

        h264unpack.SetFrameData(pData, iPacketSize, params);
    }
    else if (24 == type)
    {//STAP_A
        return 0;
    }
    else
    {//Single
        SH264PacketParams params;
        params.type = enNaluType_Single;
        params.H264NalHeader = pData[0];

        h264unpack.SetFrameData(pData + 1, iPacketSize - 1, params);

    }

    return 0;
}

void CH264Parser::ParserEndProcess()
{
    if (NULL != m_pOutputFile)
    {
        fclose(m_pOutputFile);
        m_pOutputFile = NULL;
    }
}