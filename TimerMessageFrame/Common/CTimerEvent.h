#pragma once

#include "CTimerNodeList.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include "date.h"

enum em_timer_events_state
{
    em_execute_timer = 0,     //执行定时器事件
    em_insert_timer,          //添加定时器
    em_delete_timer           //删除定时器
};

inline std::string get_curr_time()
{
    string strDate;

    auto  tt_now = system_clock::now();

    //得到毫秒值
    typedef duration<int, ratio_multiply<hours::period, ratio<24> >::type> days;
    system_clock::duration tp = tt_now.time_since_epoch();
    auto dp = date::floor<date::days>(tp);

    auto time = date::make_time(duration_cast<milliseconds>(tp - dp));

    auto tt = std::chrono::system_clock::to_time_t
              (tt_now);
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%04d-%02d-%02d-%02d.%02d.%02d:%d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)time.hours().count(), (int)time.minutes().count(), (int)time.seconds().count(), (int)time.subseconds().count());

    strDate = date;
    return strDate;
}

//timer执行事件
class CTimerEvents
{
public:
    CTimerEvents() : em_timer_events_state_(em_execute_timer), timer_id_(0), timer_interval_(0), function_arg_(nullptr)
    {

    }

    em_timer_events_state em_timer_events_state_;
    int timer_id_;
    milliseconds timer_interval_;
    TimerFunctor timer_function_;
    void*        function_arg_;
};

class CTimerThreadInfo
{
public:
    CTimerThreadInfo() : is_run_(false) {};

    void get_timer_events_list(vector<CTimerEvents>& timer_events_temp_list_)
    {
        std::lock_guard <std::mutex> lock(thread_mutex_);

        if (timer_events_list_.size() > 0)
        {
            timer_events_temp_list_.swap(timer_events_list_);
        }
    }

    CTimerNodeList timer_node_list_;
    bool is_run_;
    std::thread thread_;
    std::mutex thread_mutex_;
    mutable std::mutex thread_wait_mutex_;
    std::condition_variable condition_;
    vector<CTimerEvents> timer_events_list_;
};

class CTimerManager
{
public:
    CTimerManager();
    ~CTimerManager();

    bool add_timer(int timer_id, milliseconds timer_interval, TimerFunctor&& f, void* arg);

    bool del_timer(int timer_id);

private:
    void run();

private:
    CTimerThreadInfo timer_thread_info_;
};

