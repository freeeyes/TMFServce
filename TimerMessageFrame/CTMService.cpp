#include "CTMService.h"


CTMService::CTMService() :m_nTimerMaxCount(0), m_pMessageQueueManager(NULL)
{
}

int CTMService::Init(IMessageQueueManager* pMessageQueueManager)
{
    m_pMessageQueueManager = pMessageQueueManager;

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
        pTimerInfo->m_pMessageQueueManager = m_pMessageQueueManager;

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

    return 0;
}

void CTMService::Close()
{
    //关闭定时器
    m_tsTimer.Close();

    Sleep(100);

    //清空
    vector<CTimerInfo* > vecInfoList;
    m_HashTimerList.Get_All_Used(vecInfoList);

    for (int i = 0; i < (int)vecInfoList.size(); i++)
    {
        delete (CTimerInfo* )vecInfoList[i];
    }

    m_HashTimerList.Close();
}

int CTMService::AddMessage(string strName, ts_timer::CTime_Value tvexpire, void* pArg, int nMessageID)
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

    CEventsInfo objEventsInfo;

    objEventsInfo.m_tcExpire   = tvexpire;
    objEventsInfo.m_pArg       = pArg;
    objEventsInfo.m_nMessageID = nMessageID;

    pTimerInfo->m_vecEventsList.push_back(objEventsInfo);

    pTimerInfo->m_objMutex.UnLock();

    return 0;
}
