// TimerMessageFrame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CTMService.h"

class CTestUser
{
public:
    void DoMessage(int _message_id, void* _arg)
    {
        printf("[CTestUser::DoMessage]****_message_id=%d.\n", _message_id);
    }
};


int main()
{
    CTMService tm;
    tm.Init();
    CTestUser _test_user;

    //测试添加一个消息
    ts_timer::CTime_Value tvNow = ts_timer::GetTimeofDay();
    ts_timer::CTime_Value tvInterval(1, 0);

    ts_timer::CTime_Value tvexpire = tvNow + tvInterval;

    if (0 != tm.AddMessageClass("test1", 1, 0, &_test_user, &CTestUser::DoMessage, 1, NULL))
    {
        PRINTF("[main]AddMessage error.\n");
    }

    getchar();

    tm.Close();
    return 0;
}

/*
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
*/

