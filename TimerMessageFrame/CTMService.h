#pragma once

#include "TimerThread.h"
#include "XmlOpeation.h"

#include <vector>

using namespace std;

const char XML_CONF_FILE[] = "Timer.xml";

class CTimerInfo
{
public:
    char m_szName[100];
    int  m_nInterval;
    int  m_nMaxQueueList;

    CTimerInfo()
    {
        m_szName[0]     = '\0';
        m_nInterval     = 0;
        m_nMaxQueueList = 0;
    }
};

class CTMService
{
public:
    CTMService();

    int Init();

    void Close();

    //ÃÌº”œ˚œ¢
    int AddMessage(const char* pName, int nInterval, void* pArg);

private:
    ts_timer::CTimerThread m_tsTimer;
    int                    m_nTimerMaxCount;
    vector<CTimerInfo>     m_objTimerInfo;
};

