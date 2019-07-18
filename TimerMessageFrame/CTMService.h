#pragma once

#include "TSCommon.h"

class CTMService
{
public:
    CTMService();

    int Init(IMessageQueueManager* pMessageQueueManager);

    void Close();

    //�����Ϣ
    int AddMessage(string pName, ts_timer::CTime_Value tvexpire, void* pArg, int nMessageID);

private:
    ts_timer::CTimerThread m_tsTimer;
    int                    m_nTimerMaxCount;
    CHashTable<CTimerInfo> m_HashTimerList;
    IMessageQueueManager*  m_pMessageQueueManager;
};

