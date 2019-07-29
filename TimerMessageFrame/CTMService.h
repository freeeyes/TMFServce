#pragma once

#include "TSCommon.hpp"

#ifdef WIN32

#ifdef TIMERLIB_BUILD_DLL
#define TIME_DECLDIR __declspec(dllexport)
#else
#define TIME_DECLDIR __declspec(dllimport)
#endif

#endif //WIN32

class CTMService;

class CTMService
{
public:
    CTMService();

    int Init();

    void Close();

    template<typename TYPE>
    int AddMessageClass(string strName, long sec, long usec, TYPE* my_obj_ptr, void(TYPE::*handler)(int, void*), int msg_id, void* arg)
    {
        auto functor = std::bind(handler, my_obj_ptr, std::placeholders::_1, std::placeholders::_2);
        return AddMessage(strName, sec, usec, std::move(functor), msg_id, arg);
    }

    int AddMessage(string strName, long sec, long usec, CMessageInfo::UserFunctor&& f, int _Message_id, void* _arg);

private:
    ts_timer::CTimerThread     m_tsTimer;
    int                        m_nTimerMaxCount;
    CHashTable<CTimerInfo>     m_HashTimerList;
    CThreadQueueManager        m_ThreadQueueManager;
    unordered_map<int, int>    m_T2MList;
    unordered_map<int, int>    m_M2TList;
};