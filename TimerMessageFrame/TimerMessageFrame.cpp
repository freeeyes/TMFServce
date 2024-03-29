// TimerMessageFrame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CTMService.h"

class CTestUser : public IMessagePrecess
{
public:
    void DoMessage(int _message_id, void* _arg)
    {
        printf("[CTestUser::DoMessage]****_message_id=%d.\n", _message_id);
    }
};


#ifndef TIMERLIB_BUILD_DLL

int main()
{
    CTMService tm;
    CTestUser _test_user;
    tm.Init();

    //测试添加一个消息
    Enum_Timer_Mode emTimerMode = Timer_Mode_Run_Once;

    if (-1 == tm.AddMessage("test1", 1234, 1, 0, 1000, NULL, emTimerMode, (IMessagePrecess* )&_test_user))
    {
        PRINTF("[main]AddMessage error.\n");
    }

    this_thread::sleep_for(std::chrono::milliseconds(2000));

    tm.Close();
    return 0;
}

#else


BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        printf("\nprocess attach of dll");
        break;

    case DLL_THREAD_ATTACH:
        printf("\nthread attach of dll");
        break;

    case DLL_THREAD_DETACH:
        printf("\nthread detach of dll");
        break;

    case DLL_PROCESS_DETACH:
        printf("\nprocess detach of dll");
        break;
    }

    return TRUE;
}

#endif