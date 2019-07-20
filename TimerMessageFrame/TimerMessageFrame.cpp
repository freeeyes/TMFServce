// TimerMessageFrame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CTMService.h"

//int main()
//{
//    CTMService tm;
//    tm.Init(NULL);
//
//    //测试添加一个消息
//    ts_timer::CTime_Value tvNow = ts_timer::GetTimeofDay();
//    ts_timer::CTime_Value tvInterval(1, 0);
//
//    ts_timer::CTime_Value tvexpire = tvNow + tvInterval;
//
//    if (0 != tm.AddMessage("test1", tvexpire, NULL, 1))
//    {
//        PRINTF("[main]AddMessage error.\n");
//    }
//
//    getchar();
//
//    tm.Close();
//    return 0;
//}

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

