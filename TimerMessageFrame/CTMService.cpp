#include "CTMService.h"

void timer_run_execute(void* arg)
{
    auto tt_now = system_clock::now();

    CTimerInfo* timer_info = (CTimerInfo*)arg;
    cout << "[timer_run_execute]" << timer_info->m_nID << endl;

    timer_info->m_objMutex.Lock();

    //循环比较是否到期
    for (vecEventsList::iterator it = timer_info->m_vecEventsList.begin(); it != timer_info->m_vecEventsList.end();)
    {
        if ((*it).m_ttNextTime <= tt_now)
        {
            //到时的数据，拿出来处理
            std::cout << "[CTaskTimeNode::Run]("<< timer_info->m_szName.c_str() << ") is Arrived.\n" << endl;

            if ((*it).m_pMessageQueueManager != NULL && Message_Run == (*it).m_emMessageState)
            {
                //输出到消息队列(消息)
                (*it).m_pMessageQueueManager->SendLogicThreadMessage((*it).m_nMessageID, (*it).m_pArg);
            }
            else if (NULL != timer_info->m_pMessageQueueManager && Message_Run == (*it).m_emMessageState)
            {
                //输出到消息队列(lamba)
                timer_info->m_pMessageQueueManager->AddMessage((*it).m_nWorkThreadID,
                        std::move((*it).fn),
                        (*it).m_nMessageID,
                        (*it).m_pArg);
            }
            else
            {
                //打印日志
                std::cout << "[CTaskTimeNode::Run]("<< (*it).m_nMessageID << ") is disposed.\n" << endl;
            }

            if ((*it).m_emTimerMode == Timer_Mode_Run_Once)
            {
                it = timer_info->m_vecEventsList.erase(it);
            }
            else
            {
                //如果是定时执行需求，重新计算下一次的执行时间。
                system_clock::time_point ttNextTime = tt_now + seconds((*it).m_nSec) + milliseconds((*it).m_nUsec);
                (*it).m_ttNextTime = ttNextTime;
            }
        }
        else
        {
            ++it;
        }
    }

    timer_info->m_objMutex.UnLock();
}

CTMService::CTMService() :m_nTimerMaxCount(0), m_pMessageQueueManager(NULL)
{
}

int CTMService::Init()
{
    //读取配置文件
    CXmlOpeation objXmlOperation;

    objXmlOperation.Init(XML_CONF_FILE);

    objXmlOperation.Read_XML_Data_Single_Int("Info", "TimerCount", m_nTimerMaxCount);

    TiXmlElement* pID       = NULL;
    TiXmlElement* pName     = NULL;
    TiXmlElement* pInterval = NULL;
    TiXmlElement* pMaxEvent = NULL;

    m_HashTimerList.Init(m_nTimerMaxCount);

    string szName;
    int  nID           = 0;
    int  nInterval     = 0;
    int  nMaxQueueList = 0;

    while (objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "EventID", nID, pID)
           && objXmlOperation.Read_XML_Data_Multiple_String("Timer", "Name", szName, pName)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "Interval", nInterval, pInterval)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "EventMaxCount", nMaxQueueList, pMaxEvent))
    {
        //写入配置文件
        CTimerInfo* pTimerInfo = new CTimerInfo();

        pTimerInfo->m_nID                  = nID;
        pTimerInfo->m_szName               = szName;
        pTimerInfo->m_nInterval            = nInterval;
        pTimerInfo->m_nMaxQueueList        = nMaxQueueList;
        pTimerInfo->m_pMessageQueueManager = &m_ThreadQueueManager;

        if (0 > m_HashTimerList.Add_Hash_Data(pTimerInfo->m_szName.c_str(), pTimerInfo))
        {
            PRINTF("[CTMService::Init]Add_Hash_Data_By_Key_Unit32(%s) error.\n", pTimerInfo->m_szName.c_str());
            delete pTimerInfo;
        }
    }

    if (m_HashTimerList.Get_Used_Count() > m_nTimerMaxCount)
    {
        return -1;
    }

    this_thread::sleep_for(std::chrono::milliseconds(100));

    vector<CTimerInfo* > vecInfoList;
    m_HashTimerList.Get_All_Used(vecInfoList);


    //添加定时器
    int nTimerSize = (int)vecInfoList.size();

    for (int i = 0; i < nTimerSize; i++)
    {
        milliseconds timer_interval = milliseconds(vecInfoList[i]->m_nInterval);
        timer_events_.add_timer(vecInfoList[i]->m_nID, timer_interval, timer_run_execute, &vecInfoList[i]);
    }

    //获得当前工作线程配置列表
    m_T2MList.clear();
    m_M2TList.clear();

    TiXmlElement* pWorkThreadID = NULL;
    TiXmlElement* pMessageID    = NULL;

    int nWorkThreadID = 0;
    int nMessageID    = 0;
    char szT2MID[50] = { '\0' };

    while (objXmlOperation.Read_XML_Data_Multiple_Int("MessageInfo", "LogicThreadID", nWorkThreadID, pWorkThreadID)
           && objXmlOperation.Read_XML_Data_Multiple_Int("MessageInfo", "MessageID", nMessageID, pMessageID))
    {
        //写入配置文件
        bool blIsAdd = true;

        unordered_map<int, int>::iterator ftm = m_M2TList.find(nMessageID);

        if (m_M2TList.end() == ftm)
        {
            m_M2TList.insert(std::make_pair(nMessageID, nWorkThreadID));
        }

        unordered_map<int, int>::iterator fmt = m_T2MList.find(nWorkThreadID);

        if (m_T2MList.end() == fmt)
        {
            m_T2MList.insert(std::make_pair(nWorkThreadID, nMessageID));
        }
    }

    //根据配置文件创建对应函数
    for (auto it = m_T2MList.begin(); it != m_T2MList.end(); ++it)
    {
        m_ThreadQueueManager.Create(it->first);
    }

    return 0;
}

void CTMService::SetMessageQueue(IMessageQueueManager* pMessageQueueManager)
{
    m_pMessageQueueManager = pMessageQueueManager;
}

void CTMService::Close()
{
    //清空
    vector<CTimerInfo* > vecInfoList;
    m_HashTimerList.Get_All_Used(vecInfoList);

    for (int i = 0; i < (int)vecInfoList.size(); i++)
    {
        delete (CTimerInfo* )vecInfoList[i];
    }

    m_HashTimerList.Close();

    m_ThreadQueueManager.Close();

    this_thread::sleep_for(std::chrono::milliseconds(100));
}

int CTMService::AddMessage(string strName, int nMessagePos, long sec, long usec, int _Message_id, void* _arg, Enum_Timer_Mode emTimerMode)
{
    CTimerInfo* pTimerInfo = m_HashTimerList.Get_Hash_Box_Data(strName.c_str());

    if (NULL == pTimerInfo)
    {
        return -1;
    }

    pTimerInfo->m_objMutex.Lock();

    if (pTimerInfo->m_vecEventsList.size() >= pTimerInfo->m_nMaxQueueList)
    {
        pTimerInfo->m_objMutex.UnLock();
        return -1;
    }

    unordered_map<int, int>::iterator ftm = m_M2TList.find(_Message_id);

    if (m_M2TList.end() == ftm)
    {
        pTimerInfo->m_objMutex.UnLock();
        return -1;
    }

    CEventsInfo objEventsInfo;

    system_clock::time_point ttNextTime = system_clock::now() + seconds(sec) + milliseconds(usec);

    objEventsInfo.m_ttNextTime           = ttNextTime;
    objEventsInfo.m_pArg                 = _arg;
    objEventsInfo.m_nMessageID           = _Message_id;
    objEventsInfo.m_nWorkThreadID        = ftm->second;
    objEventsInfo.m_pMessageQueueManager = m_pMessageQueueManager;
    objEventsInfo.m_nMessagePos          = nMessagePos;
    objEventsInfo.m_nSec                 = sec;
    objEventsInfo.m_nUsec                = usec;
    objEventsInfo.m_emTimerMode          = emTimerMode;

    pTimerInfo->m_vecEventsList.push_back(objEventsInfo);

    pTimerInfo->m_objMutex.UnLock();

    return objEventsInfo.m_nMessagePos;
}

int CTMService::DeleteMessage(string strName, int nMessagePos)
{
    CTimerInfo* pTimerInfo = m_HashTimerList.Get_Hash_Box_Data(strName.c_str());

    if (NULL == pTimerInfo)
    {
        return -1;
    }

    pTimerInfo->m_objMutex.Lock();

    for (int i = 0; i < (int)pTimerInfo->m_vecEventsList.size(); i++)
    {
        if (pTimerInfo->m_vecEventsList[i].m_nMessagePos == nMessagePos)
        {
            pTimerInfo->m_vecEventsList[i].m_emMessageState = Message_Cancel;
            break;
        }
    }

    pTimerInfo->m_objMutex.UnLock();

    return 0;
}


ITMService* CreateCTMService(IMessageQueueManager*& pMessageQueueManager)
{
    if (nullptr == pMessageQueueManager)
    {
        return nullptr;
    }

    CTMService* p = new CTMService();
    p->Init();
    p->SetMessageQueue(pMessageQueueManager);

    return nullptr;
}

void DsetroyCTMService(ITMService*& pTM)
{
    CTMService* p = dynamic_cast<CTMService*>(pTM);

    if (nullptr != p)
    {
        p->Close();
        delete p;
    }
}