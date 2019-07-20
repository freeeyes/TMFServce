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
	/*����һ������ʱ����õ���Ϣ����Ӧtime.xml������
	����:	pName ��Ӧ time.xml�е�Name�ֶΡ�����ʹ�����ġ�
			sec �� 
			usec ���롣 ��1��200���� sec = 1 usec = 200
			pArg ��Ĳ�����ַ�����ջᵽ���Լ��Ļص���ȥִ��
			nMessageID ��Ϣ���͡�ӳ�䵽��Ӧ��ҵ���߳�LogicQueue�е�Run����ִ�С�
	*/
	virtual int AddMessage(const char* pName, long sec, long usec, void* pArg, int nMessageID) = 0;
};

class CTMService : public ITMService
{
public:
    CTMService();

    int Init(IMessageQueueManager* pMessageQueueManager);

    void Close();

    //�����Ϣ
	virtual int AddMessage(const char* pName, long sec, long usec, void* pArg, int nMessageID) override;
private:
    int _AddMessage(string pName, ts_timer::CTime_Value tvexpire, void* pArg, int nMessageID);
private:
    ts_timer::CTimerThread m_tsTimer;
    int                    m_nTimerMaxCount;
    CHashTable<CTimerInfo> m_HashTimerList;
    IMessageQueueManager*  m_pMessageQueueManager;
};