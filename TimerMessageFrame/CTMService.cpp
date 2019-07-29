#include "CTMService.h"


CTMService::CTMService() :m_nTimerMaxCount(0)
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

    //初始化定时器
    m_tsTimer.Init(m_nTimerMaxCount);
    m_tsTimer.Run();

    Sleep(100);

    vector<CTimerInfo* > vecInfoList;
    m_HashTimerList.Get_All_Used(vecInfoList);

    for (int i = 0; i < (int)vecInfoList.size(); i++)
    {
        CTaskTimeNode* pTimeNode = new CTaskTimeNode();

        pTimeNode->SetTimerID(vecInfoList[i]->m_nID);
        pTimeNode->SetFrequency(vecInfoList[i]->m_nInterval);

        if (false == m_tsTimer.Add_Timer((ts_timer::ITimeNode*)pTimeNode, (void*)vecInfoList[i]))
        {
            PRINTF("[CTMService::Init]m_tsTimer.Add_Timer(%s) is error.\n", vecInfoList[i]->m_szName.c_str());
        }
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

void CTMService::Close()
{
    //关闭定时器
    m_tsTimer.Close();

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

int CTMService::AddMessage(string strName, long sec, long usec, CMessageInfo::UserFunctor&& f, int _Message_id, void* _arg)
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
        return -1;
    }

    CEventsInfo objEventsInfo;

    ts_timer::CTime_Value tvexpire = ts_timer::CTime_Value(sec, usec);

    objEventsInfo.m_tcExpire      = ts_timer::GetTimeofDay() + tvexpire;
    objEventsInfo.m_pArg          = _arg;
    objEventsInfo.m_nMessageID    = _Message_id;
    objEventsInfo.m_nWorkThreadID = ftm->second;
    objEventsInfo.fn              = std::move(f);

    pTimerInfo->m_vecEventsList.push_back(objEventsInfo);

    pTimerInfo->m_objMutex.UnLock();

    return 0;
}
