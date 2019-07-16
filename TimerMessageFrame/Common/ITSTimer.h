#ifndef _TSTIMER_H
#define _TSTIMER_H

#include "TimerInfo.h"

class ITSTimerManager
{
public:
    virtual ~ITSTimerManager() {};

    //��Ӷ�ʱ��
    virtual bool Add_Timer(ts_timer::ITimeNode* pTimeNode, void* pArgContext) = 0;

    //ɾ����ʱ��
    virtual bool Del_Timer(int nTimerID) = 0;

    //��ͣ��ʱ��
    virtual bool Pause() = 0;

    //�ָ���ͣ��ʱ��
    virtual bool Restore() = 0;
};

#endif
