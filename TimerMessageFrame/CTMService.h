#pragma once

#include "TSCommon.hpp"
#include "ITMService.h"

class CTMService : public ITMService
{
public:
    CTMService();

    int Init();

    void SetMessageQueue(IMessageQueueManager* pMessageQueueManager);

    void Close();

    int AddMessage(string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once);

    int DeleteMessage(string strName, int nMessagePos);

private:
    CTimerManager              timer_events_;
    int                        m_nTimerMaxCount;
    CHashTable<CTimerInfo>     m_HashTimerList;
    CThreadQueueManager        m_ThreadQueueManager;
    unordered_map<int, int>    m_T2MList;
    unordered_map<int, int>    m_M2TList;
    IMessageQueueManager*      m_pMessageQueueManager;
};