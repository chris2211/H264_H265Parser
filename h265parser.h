#ifndef H265_PARSER_H
#define H265_PARSER_H
#include <stdio.h>
#include <string>
#include "typedef.h"
#include "h265frameunpack.h"

class CH265Parser
{
    public:
        static CH265Parser* GetInstance();
        static void Release();

    public:
        int Init(int iPayloadType,const std::string& strSSRC, std::string& strOutputFile,
            const std::string& strSrcIP = "", int iSrcPort = 0, const std::string& strDstIP = "", int iDstPort = 0);

        void ParserH265(const SRTPInfo& rtpInfo, unsigned char* pData, int iPayloadSize);
        void ParserEndProcess();
    private:
        CH265Parser() :m_pOutputFile(NULL), m_iPayLoadType(-1)
        {
#ifdef H265_DEBUG
            m_pH265File = fopen("h265_debug_file","wb");
#endif
        }

        ~CH265Parser()
        {
            if (NULL != m_pOutputFile)
            {
                fclose(m_pOutputFile);
                m_pOutputFile = NULL;
            }

#ifdef H265_DEBUG
            if (NULL != m_pH265File)
            {
                fclose(m_pH265File);
                m_pH265File = NULL;
            }
#endif
        }

    private:
        int ProcessH265Video(unsigned char* pData, unsigned int iSize, bool isMark, CH265FrameUnpack &h264unpack);
    
    private:
        static CH265Parser *m_pInstance;

    private:
    
        FILE *m_pOutputFile;

        int m_iPayLoadType;
        std::string m_strSSRC;
        std::string m_strSrcIP;
        int m_iSrcPort;
        std::string m_strDstIP; 
        int m_iDstPort;

        CH265FrameUnpack m_h265Frame;
#ifdef H265_DEBUG
        FILE *m_pH265File;
#endif
        int m_iCurretnKeyFrame;

#ifdef PACKET_CHECK
        //用于对包的兼容处理
        enFUAState m_enFUAState;
#endif

};
#endif
