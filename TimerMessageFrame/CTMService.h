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
class ITMService;

extern "C"
{
	TIME_DECLDIR ITMService* CreateCTMService(IMessageQueueManager* pMessageQueueManager);
	TIME_DECLDIR void DsetroyCTMService(ITMService* pTM);
}

class TIME_DECLDIR ITMService
{
public:
	/*增加一个到期时间调用的消息，对应time.xml的配置
	参数:	pName 对应 time.xml中的Name字段。不能使用中文。
			sec 秒 
			usec 毫秒。 如1秒200毫秒 sec = 1 usec = 200
			pArg 你的参数地址。最终会到你自己的回调中去执行
			nMessageID 消息类型。映射到对应的业务线程LogicQueue中的Run函数执行。
	*/
	virtual int AddMessage(const char* pName, long sec, long usec, void* pArg, int nMessageID) = 0;
};

class CTMService : public ITMService
{
public:
    CTMService();

    int Init(IMessageQueueManager* pMessageQueueManager);

    void Close();

    //添加消息
	virtual int AddMessage(const char* pName, long sec, long usec, void* pArg, int nMessageID) override;
private:
    int _AddMessage(string pName, ts_timer::CTime_Value tvexpire, void* pArg, int nMessageID);
private:
    ts_timer::CTimerThread m_tsTimer;
    int                    m_nTimerMaxCount;
    CHashTable<CTimerInfo> m_HashTimerList;
    IMessageQueueManager*  m_pMessageQueueManager;
};