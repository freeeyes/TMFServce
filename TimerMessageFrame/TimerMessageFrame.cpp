// TimerMessageFrame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CTMService.h"

int main()
{
    CTMService tm;
    tm.Init(NULL);

    //测试添加一个消息
    ts_timer::CTime_Value tvNow = ts_timer::GetTimeofDay();
    ts_timer::CTime_Value tvInterval(1, 0);

    ts_timer::CTime_Value tvexpire = tvNow + tvInterval;

    if (0 != tm.AddMessage("test1", tvexpire, NULL, 1))
    {
        PRINTF("[main]AddMessage error.\n");
    }

    getchar();

    tm.Close();
    return 0;
}

