#ifndef _TSCOMMON_H
#define _TSCOMMON_H

#include "CTimerEvent.h"
#include "XmlOpeation.h"
#include "ThreadLock.h"
#include "HashTable.h"
#include "ThreadLogic.h"
#include "IMessageQueueManager.h"
#include <vector>

using namespace std;

const char XML_CONF_FILE[] = "Timer.xml";

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
    system_clock::time_point    m_ttNextTime;             //下一次执行时间
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

#endif
