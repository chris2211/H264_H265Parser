#ifndef H264_PARSER_H
#define H264_PARSER_H
#include <string>
#include "typedef.h"
#include "h264frameunpack.h"

class CH264Parser
{
public:
    static CH264Parser* GetInstance();
    static void Release();

public:
    int Init(int iPayloadType, const std::string& strSSRC, std::string& strOutputFile,
        const std::string& strSrcIP = "", int iSrcPort = 0, const std::string& strDstIP = "", int iDstPort = 0);

    void ParserH264(const SRTPInfo& rtpInfo, unsigned char* pData, int iPayloadSize);
    void ParserEndProcess();
private:
    CH264Parser() :m_pOutputFile(NULL), m_iPayLoadType(-1)
    {
    }

    ~CH264Parser()
    {
        if (NULL != m_pOutputFile)
        {
            fclose(m_pOutputFile);
            m_pOutputFile = NULL;
        }
    }

private:
    int ProcessH264Video(unsigned char* pData, unsigned int iSize, bool isMark, CH264FrameUnpack &h264unpack);

private:
    static CH264Parser *m_pInstance;

private:

    FILE *m_pOutputFile;

    int m_iPayLoadType;
    std::string m_strSSRC;
    std::string m_strSrcIP;
    int m_iSrcPort;
    std::string m_strDstIP;
    int m_iDstPort;

    CH264FrameUnpack m_h264Frame;

    int m_iCurretnKeyFrame;

};

#endif
