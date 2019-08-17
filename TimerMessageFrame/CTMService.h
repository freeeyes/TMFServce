#pragma once

#include "TSCommon.hpp"
#include "ITMService.h"

class IA
{
public:
	void CallFunc(IA* pa)
	{
		this->Func();
	}
	void Func()
	{}
};


class ITMService;

class CTMService : public ITMService
{
public:
    CTMService();

    int Init();

    void SetMessageQueue(IMessageQueueManager* pMessageQueueManager);

    void Close();

    template<typename TYPE>
    int AddMessageClass(string strName, int nMessagePos, long sec, long usec, TYPE* my_obj_ptr, void(TYPE::*handler)(int, void*), int msg_id, void* arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once)
    {
        auto functor = std::bind(handler, my_obj_ptr, std::placeholders::_1, std::placeholders::_2);
        return AddMessage(strName, nMessagePos, sec, usec, std::move(functor), msg_id, arg, Timer_Mode_Run_Once);
    }

    int AddMessage(string strName, int nMessagePos, long sec, long usec, CMessageInfo::UserFunctor&& f, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once);

    int AddMessage(string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once) override;

    void* DeleteMessage(string strName, int nMessagePos) override;

private:
    ts_timer::CTimerThread     m_tsTimer;
    int                        m_nTimerMaxCount;
    CHashTable<CTimerInfo>     m_HashTimerList;
    CThreadQueueManager        m_ThreadQueueManager;
    unordered_map<int, int>    m_T2MList;
    unordered_map<int, int>    m_M2TList;
    Enum_Message_Mode          m_emMessageMode;
    IMessageQueueManager*      m_pMessageQueueManager;
};