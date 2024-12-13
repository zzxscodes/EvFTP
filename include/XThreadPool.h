#pragma once
#include <vector>
class XThread;
class XTask;
class XThreadPool
{
public:
    static XThreadPool* Get()
    {
        static XThreadPool p;
        return &p;
    }
    //初始化线程
    void Init(int threadCount);
    //分发线程
    void Dispatch(XTask* task);
private:
    int threadCount = 0;
    int lastThread = -1;
    std::vector<XThread*>threads;
    XThreadPool(){}
};