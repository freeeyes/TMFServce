#pragma once

#include <string>

#if PSS_PLATFORM == PLATFORM_WIN
#ifdef TIMERLIB_BUILD_DLL
#define TIME_DECLDIR __declspec(dllexport)
#else
#define TIME_DECLDIR __declspec(dllimport)
#endif
#else
#define TIME_DECLDIR
#endif

class ITMService;
class IMessageQueueManager;
enum Enum_Timer_Mode;

extern "C"
{
    TIME_DECLDIR ITMService* CreateCTMService(IMessageQueueManager*& pMessageQueueManager);
    TIME_DECLDIR void DsetroyCTMService(ITMService*& pTM);
}

class TIME_DECLDIR ITMService
{
public:
    virtual int AddMessage(std::string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once, IMessagePrecess* pMessagePrecess = NULL) = 0;

    virtual void* DeleteMessage(std::string strName, int nMessagePos) = 0;
};

