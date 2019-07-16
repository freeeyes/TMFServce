#pragma once

#include "TimerThread.h"
#include "XmlOpeation.h"
#include "ThreadLock.h"
#include "HashTable.h"
#include <vector>

using namespace std;

const char XML_CONF_FILE[] = "Timer.xml";

class CEventsInfo
{
public:
    ts_timer::CTime_Value m_tcExpire;
    void*                 m_pArg;

    CEventsInfo()
    {
        m_pArg = NULL;
    }
};

typedef vector<CEventsInfo> vecEventsList;

//定时器信息结构
class CTimerInfo
{
public:
    int  m_nID;
    char m_szName[100];
    int  m_nInterval;
    int  m_nMaxQueueList;
    CThreadLock   m_objMutex;
    vecEventsList m_vecEventsList;

    CTimerInfo()
    {
        m_nID           = 0;
        m_szName[0]     = '\0';
        m_nInterval     = 0;
        m_nMaxQueueList = 0;
    }
};

//定时器消息处理
class CTaskTimeNode : public ts_timer::ITimeNode
{
public:
    CTaskTimeNode() {};

    virtual ~CTaskTimeNode() = default;

    virtual void Run(ts_timer::CTime_Value& tvNow, void* pArg, ts_timer::EM_Timer_State& emState)
    {
        CTimerInfo* pTimeInfo = (CTimerInfo*)pArg;

        pTimeInfo->m_objMutex.Lock();

        //循环比较是否到期
        for (vecEventsList::iterator it = pTimeInfo->m_vecEventsList.begin(); it != pTimeInfo->m_vecEventsList.end();)
        {
            if ((*it).m_tcExpire <= tvNow)
            {
                //到时的数据，拿出来处理
                printf_s("[CTaskTimeNode::Run](%s) is Arrived.\n", pTimeInfo->m_szName);
                pTimeInfo->m_vecEventsList.erase(it);
            }
            else
            {
                ++it;
            }
        }

        pTimeInfo->m_objMutex.UnLock();

        printf_s("[CTaskTimeNode::Run](%s) is OK.\n", pTimeInfo->m_szName);
    }

    virtual void Error(int nLastRunTimerID, int nTimeoutTime, std::vector<ts_timer::CTime_Value>& vecTimoutList, void* pArg)
    {
        return;
    }
};

class CTMService
{
public:
    CTMService();

    int Init();

    void Close();

    //添加消息
    int AddMessage(string pName, ts_timer::CTime_Value tvexpire, void* pArg, int nMessageID);

private:
    ts_timer::CTimerThread m_tsTimer;
    int                    m_nTimerMaxCount;
    CHashTable<CTimerInfo> m_HashTimerList;
};

