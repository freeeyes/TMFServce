#ifndef ILOGICQUEUE_H
#define ILOGICQUEUE_H

#include <string>

//add �빤���߳��޹ص�ҵ����Ϣ���е��߼�ҵ����,����̳к�ʵ�� liuruiqi
enum ThreadReturn
{
    THREAD_Task_Exit = 0,    //��ǰ�߳��˳�
    THREAD_Task_Finish,      //��ǰ�߳��������
};

enum ThreadError
{
    THREAD_Error_Timeout = 0,    //�߳�����
};

class ILogicQueue
{
public:
    //��ʼ������ ����:�߼�ID, ����, ���ʱ��(��)
    ILogicQueue(int u4LogicThreadID, int u4Timeout, std::string& strDesc);
    virtual ~ILogicQueue();
public:
    //��ȡ�߳�ID
    int GetLogicThreadID();
    //��ʱ
    void SetTimeOut(int u4Timeout);
    int GetTimeOut();
    //����
    std::string GetDescriptor();

public:
    //��ʼ������
    virtual bool Init() = 0;
    //ִ���߼�
    virtual ThreadReturn Run(int nMessage, void* arg) = 0;
    //������
    virtual int Error(int nErrorID) = 0;
    //�˳��ƺ�
    virtual void Exit() = 0;
private:
    ILogicQueue& operator=(const ILogicQueue&) = delete;
private:
    int      m_u4LogicThreadID;
    int      m_u4Timeout;
    std::string m_strDesc;
};


#endif // ILOGIC_H