#pragma once

#include "TSCommon.hpp"
#include "ITMService.h"

#ifndef TIMERLIB_BUILD_DLL
class CTMService
#else
class CTMService : public ITMService
#endif
{
public:
    CTMService();

    int Init();

    void SetMessageQueue(IMessageQueueManager* pMessageQueueManager);

    void Close();
#ifndef TIMERLIB_BUILD_DLL
    int AddMessage(string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once);
    int DeleteMessage(string strName, int nMessagePos);
#else
	int AddMessage(string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once) override;
	int DeleteMessage(string strName, int nMessagePos) override;
#endif

private:
    CTimerManager              timer_events_;
    int                        m_nTimerMaxCount;
    CHashTable<CTimerInfo>     m_HashTimerList;
    CThreadQueueManager        m_ThreadQueueManager;
    unordered_map<int, int>    m_T2MList;
    unordered_map<int, int>    m_M2TList;
    IMessageQueueManager*      m_pMessageQueueManager;
};