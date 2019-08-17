#ifndef _TSCOMMON_H
#define _TSCOMMON_H

#include "TimerThread.h"
#include "XmlOpeation.h"
#include "ThreadLock.h"
#include "HashTable.h"
#include "ThreadLogic.h"
#include "IMessageQueueManager.h"
#include <vector>

using namespace std;

const char XML_CONF_FILE[] = "Timer.xml";

enum Enum_Message_Mode
{
    Lambda_Mode = 0,
    Message_Mode,
};

enum Enum_Timer_Mode
{
    Timer_Mode_Run_Once = 0,
    Timer_Mode_Interval,
};

enum Enum_Message_Execute_State
{
    Message_Run = 0,
    Message_Cancel,
};

class CEventsInfo
{
public:
    ts_timer::CTime_Value       m_tcExpire;
    void*                       m_pArg;
    int                         m_nMessageID;
    int                         m_nWorkThreadID;
    int                         m_nMessagePos;
    int                         m_nSec;                    //time of interval
    int                         m_nUsec;
    Enum_Message_Execute_State  m_emMessageState;
    Enum_Timer_Mode             m_emTimerMode;

    CMessageInfo::UserFunctor   fn;
    IMessageQueueManager*       m_pMessageQueueManager;

    CEventsInfo() : m_pArg(NULL), m_nMessageID(0), m_nWorkThreadID(0), m_nMessagePos(0), m_nSec(0), m_nUsec(0), m_emMessageState(Message_Run), m_emTimerMode(Timer_Mode_Run_Once), m_pMessageQueueManager(NULL)
    {
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
    CThreadQueueManager* m_pMessageQueueManager;

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

                if ((*it).m_pMessageQueueManager != NULL && Message_Run == (*it).m_emMessageState)
                {
                    //输出到消息队列(消息)
                    (*it).m_pMessageQueueManager->SendLogicThreadMessage((*it).m_nMessageID, (*it).m_pArg);
                }
                else if (NULL != pTimeInfo->m_pMessageQueueManager && Message_Run == (*it).m_emMessageState)
                {
                    //输出到消息队列(lamba)
                    pTimeInfo->m_pMessageQueueManager->AddMessage((*it).m_nWorkThreadID,
                            std::move((*it).fn),
                            (*it).m_nMessageID,
                            (*it).m_pArg);
                }
                else
                {
                    //打印日志
                    PRINTF("[CTaskTimeNode::Run](%d) is disposed.\n", (*it).m_nMessageID);
                }

                if ((*it).m_emTimerMode == Timer_Mode_Run_Once)
                {
                    it = pTimeInfo->m_vecEventsList.erase(it);
                }
                else
                {
                    //如果是定时执行需求，重新计算下一次的执行时间。
                    ts_timer::CTime_Value tvexpire = ts_timer::CTime_Value((*it).m_nSec, (*it).m_nUsec);
                    (*it).m_tcExpire = tvNow + tvexpire;
                }
            }
            else
            {
                ++it;
            }
        }

        pTimeInfo->m_objMutex.UnLock();

        //printf_s("[CTaskTimeNode::Run](%s) is OK.\n", pTimeInfo->m_szName.c_str());
    }

    virtual void Error(int nLastRunTimerID, int nTimeoutTime, std::vector<ts_timer::CTime_Value>& vecTimoutList, void* pArg)
    {
        return;
    }
};

#endif
