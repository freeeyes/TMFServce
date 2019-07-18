#ifndef _TSCOMMON_H
#define _TSCOMMON_H

#include "TimerThread.h"
#include "XmlOpeation.h"
#include "ThreadLock.h"
#include "HashTable.h"
#include "IMessageQueueManager.h"
#include <vector>

using namespace std;

const char XML_CONF_FILE[] = "Timer.xml";

class CEventsInfo
{
public:
    ts_timer::CTime_Value m_tcExpire;
    void*                 m_pArg;
    int                   m_nMessageID;

    CEventsInfo()
    {
        m_pArg = NULL;
        m_nMessageID = 0;
    }
};

using vecEventsList = vector<CEventsInfo>;

//定时器信息结构
class CTimerInfo
{
public:
    int  m_nID;
    string m_szName;
    int  m_nInterval;
    int  m_nMaxQueueList;
    CThreadLock   m_objMutex;
    vecEventsList m_vecEventsList;
    IMessageQueueManager* m_pMessageQueueManager;

    CTimerInfo() : m_nID(0), m_szName{ '\0' }, m_nInterval(0), m_nMaxQueueList(0), m_pMessageQueueManager(NULL)
    {
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
                PRINTF("[CTaskTimeNode::Run](%s) is Arrived.\n", pTimeInfo->m_szName.c_str());

                if (NULL != pTimeInfo->m_pMessageQueueManager)
                {
                    //输出到消息队列
                    pTimeInfo->m_pMessageQueueManager->SendLogicThreadMessage((*it).m_nMessageID, (*it).m_pArg);
                }
                else
                {
                    //打印日志
                    PRINTF("[CTaskTimeNode::Run](%d) is disposed.\n", (*it).m_nMessageID);
                }

                it = pTimeInfo->m_vecEventsList.erase(it);
            }
            else
            {
                ++it;
            }
        }

        pTimeInfo->m_objMutex.UnLock();

        printf_s("[CTaskTimeNode::Run](%s) is OK.\n", pTimeInfo->m_szName.c_str());
    }

    virtual void Error(int nLastRunTimerID, int nTimeoutTime, std::vector<ts_timer::CTime_Value>& vecTimoutList, void* pArg)
    {
        return;
    }
};

#endif
