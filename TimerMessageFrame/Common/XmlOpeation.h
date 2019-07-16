#ifndef _XMLOPRATION_H
#define _XMLOPRATION_H

#include "tinyxml.h"
#include "tinystr.h"

#include <memory>

using namespace std;

class CXmlOpeation
{
public:
    CXmlOpeation(void);

    bool Init(const char* pFileName);

    char* GetData(const char* pName, const char* pAttrName);
    char* GetData(const char* pName, const char* pAttrName, TiXmlElement*& pNextTiXmlElement);

    //读取单个变量
    bool Read_XML_Data_Single_String(const char* pTag, const char* pName, char* pValue, int nMaxSize);
    bool Read_XML_Data_Single_Int(const char* pTag, const char* pName, int& u1Value);

    //读取循环里的变量
    bool Read_XML_Data_Multiple_String(const char* pTag, const char* pName, char* pValue, int nMaxSize, TiXmlElement*& pTi);
    bool Read_XML_Data_Multiple_Int(const char* pTag, const char* pName, int& u4Value, TiXmlElement*& pTi);

private:
    std::shared_ptr<TiXmlDocument> m_pTiXmlDocument;
    TiXmlElement*                  m_pRootElement;
};
#endif
