#ifndef _TIMER_NODE_LIST_H
#define _TIMER_NODE_LIST_H

#include <iostream>
#include <chrono>
#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

using namespace std;
using namespace std::chrono;

using TimerFunctor = std::function<void(void*)>;

//�ڵ���Ϣ
class CTimerNodeInfo
{
public:
    CTimerNodeInfo() : timer_id_(0), timer_interval_(0), function_arg_(nullptr)
    {
        steady_clock::time_point timer_now = steady_clock::now();
        begin_time_ = timer_now;
        next_time_  = timer_now;
    }

    void Step(steady_clock::time_point& timer_now)
    {
        if (next_time_ < timer_now)
        {
            //��Ҫ��ת���㵽��ǰʱ�����һ��ִ��ʱ���
            milliseconds interval = duration_cast<milliseconds>(timer_now - begin_time_);

            milliseconds begin = interval % timer_interval_;

            next_time_ = timer_now + (timer_interval_ - begin);
        }
        else
        {
            next_time_ += timer_interval_;
        }
    }

    bool operator < (const CTimerNodeInfo& ar) const
    {
        return (this->next_time_ < ar.next_time_);
    }

public:
    int timer_id_;                            //��ʱ��ID
    milliseconds timer_interval_;             //����ʱ����
    steady_clock::time_point next_time_;      //��һ��ִ��ʱ��
    steady_clock::time_point begin_time_;     //��ʱ����ʼʱ��
    TimerFunctor timer_function_;             //��ʱ��ִ�к���
    void*        function_arg_;               //ִ�к�������
};

//������Ϣ
class CRunNodeInfo
{
public:
    CRunNodeInfo() : timer_id_(0), curr_time_(steady_clock::now()), timer_interval_(0), function_arg_(nullptr)
    {
    };

    int                      timer_id_;
    steady_clock::time_point curr_time_;
    microseconds             timer_interval_;
    TimerFunctor             timer_function_;             //��ʱ��ִ�к���
    void*                    function_arg_;               //ִ�к�������
};

class CTimerNodeList
{
public:
    CTimerNodeList();
    ~CTimerNodeList();

    void add_timer_node_info(int timer_id, milliseconds interval, TimerFunctor f, void* arg);
    void del_timer_node_info(int timer_id);

    void display();

    void get_run_list(steady_clock::time_point timer_now = steady_clock::now());

    int get_next_run_timer_interval(CRunNodeInfo& run_node_info, microseconds& timer_interval);

private:
    int lcm(int num1, int num2);
    int gcd(int num1, int num2);

public:
    vector<std::shared_ptr<CTimerNodeInfo>> vec_timer_node_list_;
    vector<CRunNodeInfo> vec_timer_run_list_;
    int lcm_data_;
    int timer_run_list_index_;
};
#endif
