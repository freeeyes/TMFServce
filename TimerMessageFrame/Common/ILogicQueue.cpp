#include "ILogicQueue.h"

ILogicQueue::ILogicQueue(int u4LogicThreadID, int u4Timeout, std::string& strDesc)
    : m_u4LogicThreadID(u4LogicThreadID),
      m_u4Timeout(u4Timeout),
      m_strDesc(strDesc)
{
}

ILogicQueue::~ILogicQueue()
{
}

int ILogicQueue::GetLogicThreadID()
{
    return m_u4LogicThreadID;
}

void ILogicQueue::SetTimeOut(int u4Timeout)
{
    m_u4Timeout = u4Timeout;
}
int ILogicQueue::GetTimeOut()
{
    return m_u4Timeout;
}

std::string ILogicQueue::GetDescriptor()
{
    return m_strDesc;
}