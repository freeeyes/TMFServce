// TimerMessageFrame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CTMService.h"

int main()
{
	CTMService tm;
	tm.Init();
	tm.Close();
    getchar();
    return 0;
}

