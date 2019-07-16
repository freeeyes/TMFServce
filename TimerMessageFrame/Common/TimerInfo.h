#ifndef _TIMER_INFO_H
#define _TIMER_INFO_H

//����Timer��ִ�е�Ԫ
//add by freeeyes

#include "time.h"
#include "TimerCommon.h"
#include "Lcm.h"
#if PSS_PLATFORM == PLATFORM_WIN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#endif

//��ʱ����ض�������
//add by freeeyes

namespace ts_timer
{
#define MAX_TIMER_LIST_COUNT 10

    enum EM_Timer_State
    {
        TIMER_STATE_OK = 0,    //��ʱ����������
        TIMER_STATE_DEL,       //��ʱ��ɾ��
    };

    enum EM_Event_Type
    {
        TIMER_STOP = 0,      //ֹͣ�߳�
        TIMER_MODIFY,        //��Timer�仯��
        TIMER_DO_EVENT,      //ִ��Timer
        TIMER_PAUSE,         //��ͣ��ʱ��
        TIMER_RESTORE,       //�ָ���ͣ��ʱ��
    };

#if PSS_PLATFORM == PLATFORM_WIN
    typedef CRITICAL_SECTION TIMER_THREAD_MUTEX;
    typedef CONDITION_VARIABLE TIMER_THREAD_COND;
    typedef DWORD TIMER_THREAD_ID;
#else
    typedef pthread_mutex_t TIMER_THREAD_MUTEX;
    typedef pthread_cond_t TIMER_THREAD_COND;
    typedef pthread_t TIMER_THREAD_ID;
#endif

    //��ʱ���ڵ�����࣬���ڼ���ʵ��
    class ITimeNode
    {
    public:
        ITimeNode() : m_nTimerID(0), m_nFrequency(0), m_ttBegin(GetTimeofDay()) {};
        virtual ~ITimeNode() {};

        void SetTimerID(int nTimerID)
        {
            m_nTimerID = nTimerID;
        }

        int GetTimerID()
        {
            return m_nTimerID;
        }

        void SetFrequency(int nFrequency)
        {
            m_nFrequency = nFrequency;
        }

        int GetFrequency()
        {
            return m_nFrequency;
        }

        void SetBeginTime(CTime_Value ttBegin)
        {
            m_ttBegin = ttBegin;
        }

        CTime_Value GetBeginTime()
        {
            return m_ttBegin;
        }

        //��ʱ��ִ�к���
        virtual void Run(CTime_Value& tvNow, void* pArg, EM_Timer_State& emState) = 0;

        //��ʱ��ִ�г�ʱ����
        //nLastRunTimerID ִ��ʱ�䱻ռ�õĶ�ʱ��ID
        //nTimeoutTime ��ռ�ݵĶ�ʱ��ʱ�䳤��
        //vecTimoutList ��ռ�õĶ�ʱ��ʱ��
        virtual void Error(int nLastRunTimerID, int nTimeoutTime, std::vector<CTime_Value>& vecTimoutList, void* pArg) = 0;

    private:
        int         m_nTimerID;     //��ʱ��ID
        int         m_nFrequency;   //��ǰ��ʱ��ʱ����(ms)
        CTime_Value m_ttBegin;      //��ǰ��ʱ����Ҫ�Ŀ�ʼʱ��
    };

    class ITimerInfo
    {
    public:
        ITimerInfo();
        virtual ~ITimerInfo();

        void Set_Timer_Param(ITimeNode* pTimeNode, void* pArgContext);

        int Get_Timer_ID();

        int Get_Timer_Frequency();

        int Get_Next_Timer(CTime_Value ttNow, bool blState = false);

        CTime_Value Get_Next_Time();

        EM_Timer_State Do_Timer_Event(CTime_Value& obj_Now);

        void Do_Error_Events(int nLastRunTimerID, int nTimeoutTime, std::vector<CTime_Value>& vecTimoutList);

    private:
        CTime_Value m_ttBeginTime;                       //��ʼ��ʱ����ʱ��
        CTime_Value m_ttLastRunTime;                     //��һ�γɹ����ж�ʱ����ʱ��
        CTime_Value m_ttNextTime;                        //��һ�����е�ʱ��

        ITimeNode*  m_pTimeNode;                         //��ʱ��ID
        void*       m_pArgContext;                       //�ص�����������
    };

    //��ʱ�¼��б�
    class CTimerInfoList
    {
    public:
        CTimerInfoList();
        ~CTimerInfoList();

        TIMER_THREAD_MUTEX* Get_mutex();

        void Set_Event_Type(EM_Event_Type emEventType);

        EM_Event_Type Get_Event_Type();

        TIMER_THREAD_COND* Get_cond();

        void Set_Thread_ID(TIMER_THREAD_ID nThreadID);

        TIMER_THREAD_ID Get_Thread_ID();

        void Lock();

        void UnLock();

        void Init(int nMaxCount = MAX_TIMER_LIST_COUNT);

        void Close();

        void Set_Run(bool blRun);

        bool Get_Run();

        bool Add_Timer(ITimerInfo* pTimerInfo);

        bool Del_Timer(int nTimerID);

        std::vector<_Lcm_Info>* Get_Curr_Timer();

        void Calculation_Run_Assemble(CTime_Value obj_Now);        //���㶨ʱ��ִ�о��񼯺�

        std::vector<_Lcm_Info>* Get_Curr_Assemble();                    //��õ�ǰҪִ�е��б�

        std::vector<_Lcm_Info>* Get_Next_Assemble();                    //�����һ��Ҫִ�е��б�

        int GetCurrTimerCount();                                   //�õ���ǰ��ʱ����Count

        int GetAssembleCount();                                    //�õ���ǰ�����б������

        ITimerInfo* GetTimerInfo(int nIndex);                      //�õ�ָ����Timerָ��

    private:
        int                                  m_nCurrTimerIndex;//��¼��ǰTimerID
        int                                  m_nMaxCount;      //��ǰ��ʱ�������������
        ITimerInfo*                          m_NextRunTimer;   //��һ��Ҫ���еĶ�ʱ������
        bool                                 m_blRun;          //�Ƿ�����
        EM_Event_Type                        m_emEventType;    //��ǰ�¼�ִ��״̬
        TIMER_THREAD_ID                      m_nThreadID;
        TIMER_THREAD_MUTEX*                  m_pMutex;
        TIMER_THREAD_COND*                   m_pCond;

        std::vector<ITimerInfo*>             m_TimerList;      //��ǰ��ʱ�������б�
        std::vector<std::vector<_Lcm_Info> > m_TimerAssemble;  //ִ�ж�ʱ���ļƻ�����
    };
}

#endif
