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

    CTimerInfo* pTimerInfo = new CTimerInfo();

    m_HashTimerList.Init(m_nTimerMaxCount);

    while (objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "ID", pTimerInfo->m_nID, pID)
           && objXmlOperation.Read_XML_Data_Multiple_String("Timer", "Name", pTimerInfo->m_szName, 100, pName)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "Interval", pTimerInfo->m_nInterval, pInterval)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "EventMaxCount", pTimerInfo->m_nMaxQueueList, pMaxEvent))
    {
        if (0 >= m_HashTimerList.Add_Hash_Data_By_Key_Unit32(pTimerInfo->m_nID, pTimerInfo))
        {
            PRINTF("[CTMService::Init]Add_Hash_Data_By_Key_Unit32(%s) error.\n", pTimerInfo->m_szName);
            delete pTimerInfo;
        }

        pTimerInfo = new CTimerInfo();
    }

    delete pTimerInfo;

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
            PRINTF("[CTMService::Init]m_tsTimer.Add_Timer(%s) is error.\n", vecInfoList[i]->m_szName);
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
        delete vecInfoList[i];
    }

    m_HashTimerList.Close();
}

int CTMService::AddMessage(const char* pName, ts_timer::CTime_Value, void* pArg)
{
    return 0;
}
