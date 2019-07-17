#include "XmlOpeation.h"

CXmlOpeation::CXmlOpeation(void)
{
    m_pTiXmlDocument = std::make_shared<TiXmlDocument>();
    m_pRootElement   = NULL;
}

bool CXmlOpeation::Init(const char* pFileName)
{
    if(NULL == m_pTiXmlDocument)
    {
        return false;
    }

    if(false == m_pTiXmlDocument->LoadFile(pFileName))
    {
        return false;
    }

    //获得根元素
    m_pRootElement = m_pTiXmlDocument->RootElement();

    return true;
}

bool CXmlOpeation::Read_XML_Data_Single_String(const char* pTag, const char* pName, std::string& sValue)
{
    char* pData = GetData(pTag, pName);

    if (pData != NULL)
    {
		sValue = pData;
        return true;
    }
    else
    {
        return false;
    }
}

bool CXmlOpeation::Read_XML_Data_Multiple_String(const char* pTag, const char* pName, std::string& sValue, TiXmlElement*& pTi)
{
    char* pData = GetData(pTag, pName, pTi);

    if (pData != NULL)
    {
		sValue = pData;
        return true;
    }
    else
    {
        return false;
    }
}

bool CXmlOpeation::Read_XML_Data_Single_Int(const char* pTag, const char* pName, int& u4Value)
{
    char* pData = GetData(pTag, pName);

    if (pData != NULL)
    {
        u4Value = (int)atoi(pData);
        return true;
    }
    else
    {
        return false;
    }
}

bool CXmlOpeation::Read_XML_Data_Multiple_Int(const char* pTag, const char* pName, int& u4Value, TiXmlElement*& pTi)
{
    char* pData = GetData(pTag, pName, pTi);

    if (pData != NULL)
    {
        u4Value = (int)atoi(pData);
        return true;
    }
    else
    {
        return false;
    }
}

char* CXmlOpeation::GetData(const char* pName, const char* pAttrName)
{
    if(m_pRootElement == NULL)
    {
        return NULL;
    }

    TiXmlElement* pTiXmlElement = m_pRootElement->FirstChildElement(pName);

    if(NULL != pTiXmlElement)
    {
        return (char* )pTiXmlElement->Attribute(pAttrName);
    }

    return NULL;
}

char* CXmlOpeation::GetData( const char* pName, const char* pAttrName, TiXmlElement*& pNextTiXmlElement )
{
    if(m_pRootElement == NULL)
    {
        return NULL;
    }

    TiXmlElement* pTiXmlElement = NULL;

    if(NULL == pNextTiXmlElement)
    {
        pTiXmlElement = m_pRootElement->FirstChildElement(pName);
        pNextTiXmlElement = pTiXmlElement;
    }
    else
    {
        pTiXmlElement  = pNextTiXmlElement->NextSiblingElement();
        pNextTiXmlElement = pTiXmlElement;
    }

    if(NULL != pTiXmlElement)
    {
        return (char* )pTiXmlElement->Attribute(pAttrName);
    }

    return NULL;
}


