#include "CTMService.h"


CTMService::CTMService() :m_nTimerMaxCount(0)
{
}

int CTMService::Init()
{
    //读取配置文件
    CXmlOpeation objXmlOperation;

    m_objTimerInfo.clear();

    objXmlOperation.Init(XML_CONF_FILE);

    objXmlOperation.Read_XML_Data_Single_Int("Info", "TimerCount", m_nTimerMaxCount);

    TiXmlElement* pName = NULL;
    TiXmlElement* pInterval = NULL;
    TiXmlElement* pMaxEvent = NULL;

    CTimerInfo objTimerInfo;

    while (objXmlOperation.Read_XML_Data_Multiple_String("Timer", "Name", objTimerInfo.m_szName, 100, pName)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "Interval", objTimerInfo.m_nInterval, pInterval)
           && objXmlOperation.Read_XML_Data_Multiple_Int("Timer", "EventMaxCount", objTimerInfo.m_nMaxQueueList, pMaxEvent))
    {
        m_objTimerInfo.push_back(objTimerInfo);
    }

    if (m_objTimerInfo.size() > m_nTimerMaxCount)
    {
        return -1;
    }

    //初始化定时器
    m_tsTimer.Init(m_nTimerMaxCount);

    for (int i = 0; i < (int)m_objTimerInfo.size(); i++)
    {
        m_tsTimer.Add_Timer();
    }

    return 0;
}

void CTMService::Close()
{
    //关闭定时器
    m_tsTimer.Close();

    m_objTimerInfo.clear();
}

int CTMService::AddMessage(const char* pName, int nInterval, void* pArg)
{

}
